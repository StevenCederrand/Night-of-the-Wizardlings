#include "Pch/Pch.h"
#include "WorldObject.h"

WorldObject::WorldObject()
	: GameObject()
{
	type = 0;
}

WorldObject::WorldObject(std::string name) 
	: GameObject(name)
{
	type = 0;
}

WorldObject::~WorldObject()
{

}

void WorldObject::update(float dt)
{

}
