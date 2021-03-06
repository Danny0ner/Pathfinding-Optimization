#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Scene.h"

j1Scene::j1Scene() : j1Module()
{
	name.assign("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	if(App->map->Load("iso_walk2.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if(App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	debug_tex = App->tex->Load("maps/path2.png");
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	static iPoint origin;
	

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN && origin_selected == false) {
		if (optimizedpathfinding == false) 
			optimizedpathfinding = true;
		else optimizedpathfinding = false;
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && optimizedpathfinding == false)
	{
		if (origin_selected == true)
		{

			lastnormaltime = App->pathfinding->CreatePath(origin, p);
		
			
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && optimizedpathfinding == true)
	{
		if (origin_selected == true)
		{
			lastoptimizedtime = App->pathfinding->CreatePathOptimized(origin, p);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= floor(200.0f * dt);

	App->map->Draw();
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	std::string pathfinding;
	std::string or_selected;
	if (optimizedpathfinding == true)
	{
		pathfinding.assign("yes");
	}
	else if (optimizedpathfinding == false) {
		pathfinding.assign("no");
	}

	if (optimizedpathfinding == true)
	{
		or_selected.assign("yes");
	}
	else if (optimizedpathfinding == false) {
		or_selected.assign("no");
	}
	
	static char title[125];
	sprintf_s(title,125, "PathfindingOptimized : %s Origin Selected : %s OptPath time: %f ms NormalPath time: %f ms",pathfinding.c_str(),or_selected.c_str(), lastoptimizedtime, lastnormaltime);
	App->win->SetTitle(title);

	//App->win->SetTitle(title.c_str());

	// Debug pathfinding ------------------------------
	//int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	
	App->render->Blit(debug_tex, p.x, p.y);

	const std::vector<iPoint>* path = App->pathfinding->GetLastPath();
	
	if (path->size() != 0)
	{
		std::vector<iPoint>::const_iterator item = path->begin();

		while(item != path->end())
		{
			iPoint pos = App->map->MapToWorld(item->x, item->y);
			App->render->Blit(debug_tex, pos.x, pos.y);
			item++;
		}
	}
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
