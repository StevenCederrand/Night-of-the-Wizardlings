#include "Pch/Pch.h"
#include <Mesh/MeshMap.h>
#include "Mesh.h"

MeshMap* MeshMap::m_meshMapInstance = 0;

MeshMap::MeshMap() {}

MeshMap* MeshMap::getInstance()
{
	if (m_meshMapInstance == 0) {
		m_meshMapInstance = new MeshMap();
	}
	return m_meshMapInstance;
}

void MeshMap::cleanUp()
{
	std::map<std::string, Mesh*>::iterator it;

	for (it = m_meshMap.begin(); it != m_meshMap.end(); it++) {
		it->second->Destroy();
		delete it->second;
	}

	m_meshMap.clear();
}

bool MeshMap::existsWithName(std::string name)
{
	if (m_meshMap.find(name) != m_meshMap.end()) {
		return true;
	}
	return false;
}


void MeshMap::useByName(std::string name)
{
	//if (existsWithName(name)) {
	//	m_Meshs[name]->use();
	//}
}

Mesh* MeshMap::getMesh(std::string name)
{
	if (existsWithName(name))
	{
		return m_meshMap[name];
	}

	return nullptr;
}

Mesh* MeshMap::createMesh(std::string name, Mesh mesh)
{
	if (existsWithName(name))
	{
		return nullptr;
	}

	Mesh* newMesh = new Mesh();
	*newMesh = mesh;
	m_meshMap[name] = newMesh;
	return newMesh;
}

void MeshMap::destroy()
{
	cleanUp();
	delete m_meshMapInstance;
}
