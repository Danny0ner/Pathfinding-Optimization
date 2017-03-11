#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"

j1PathFinding::j1PathFinding() : j1Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH),width(0), height(0)
{
	name.assign("pathfinding");
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if(CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const std::vector<iPoint>* j1PathFinding::GetLastPath() const
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
std::list<PathNode>::iterator PathList::Find(const iPoint & point)
{
	std::list<PathNode>::iterator item = list.begin();
	while (item != list.end())
	{
		if (item->pos == point) {
			return item;
		}
		++item;
	}
}

iPoint PathList::Findp(const iPoint & point)
{
	std::list<PathNode>::iterator item = list.begin();
	while (item != list.end())
	{
		if (item->pos == point) {
			return item->pos;
		}
		++item;
	}
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
PathNode* PathList::GetNodeLowestScore() const
{
	PathNode* ret = nullptr;
	std::list<PathNode>::const_reverse_iterator item = list.crbegin();
	float min = 65535;
	while (item != list.crend())
	{
		if (item->Score() < min)
		{
			min = item->Score();
			ret = &item.base()._Ptr->_Prev->_Myval;
		}
		++item;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.size();

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	//north east
	cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	//north west
	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// south east
	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// south west
	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	return list_to_fill.list.size();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
float PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	if (parent->pos.DistanceManhattan(pos) == 1){
		g = parent->g + 10;
	}
	else if (parent->pos.DistanceManhattan(pos) == 2) {
		g = parent->g + 14;
	}
	else {
		g = parent->g + 1;
	}
	h = pos.DistanceTo(destination);
	return  g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------

int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	int ret = -1;
	if (IsWalkable(origin) && IsWalkable(destination))
	{
		last_path.clear();
		ret = 1;
		PathList open;
		PathList close;
		open.list.push_back(PathNode(0, origin.DistanceManhattan(destination), origin, nullptr));
		while (open.list.size() != 0)
		{

			close.list.push_back(*open.GetNodeLowestScore());
			close.list.back().on_close = true;

			open.list.remove(*open.GetNodeLowestScore());

			if (close.list.back().pos == destination)
			{
				std::list<PathNode>::const_iterator item = close.list.end();

				for (item--; item->parent != nullptr; item = close.Find(item->parent->pos))
				{
					last_path.push_back(item->pos);
				}

				last_path.push_back(close.list.begin()->pos);
				std::reverse(last_path.begin(), last_path.end());
				open.list.clear();
				close.list.clear();
				return ret = last_path.size();
			}
			else
			{
				PathList neightbords;
				close.list.back().FindWalkableAdjacents(neightbords);
				for (std::list<PathNode>::iterator item = neightbords.list.begin(); item != neightbords.list.end(); item++) {
					if (close.Findp(item->pos) == item->pos)
					{
						continue;
					}
					else if (close.Findp(item->pos) == item->pos)
					{
						PathNode temp;
						temp = open.Find(item->pos)._Ptr->_Myval;
						item->CalculateF(destination);
						if (item->g < temp.g)
						{
							open.Find(item->pos)->parent = item->parent;
						}
					}
					else
					{
						item->on_open = true;
						item->CalculateF(destination);
						open.list.push_back(item._Mynode()->_Myval);
					}
				}
				neightbords.list.clear();
			}
		}
	}
	return ret;
}

bool PathNode::operator==(const PathNode & node) const
{
	return pos == node.pos;
}

bool PathNode::operator!=(const PathNode & node) const
{
	return !operator==(node);
}