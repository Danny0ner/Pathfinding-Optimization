#include "Buildings.h"
#include "p2Point.h"

Building::Building(BUILDING_TYPE b_type, fPoint pos): Entity(BUILDING, pos), building_type(b_type)
{}

void Building::Update()
{
	AI();
	Draw();
}

void Building::AI()
{
}

void Building::Draw()
{
}
