#include "Pch/Pch.h"
#include "MapObject.h"

MapObject::MapObject() : GameObject()
{
	m_type = 0;
}

MapObject::MapObject(std::string name) : GameObject(name)
{
	m_type = 0;
	MaterialMap* matMap = MaterialMap::getInstance(); 
	/* Loop through all of the object and set the materials rim lighting value*/
	for (auto mesh : m_meshes) {
		//Set the rimlighting value
		matMap->getMaterial(MeshMap::getInstance()->getMesh(mesh.name)->getMaterial())->rimLighting = 0;
	}
}

MapObject::~MapObject()
{
}

void MapObject::update(float dt)
{
}
