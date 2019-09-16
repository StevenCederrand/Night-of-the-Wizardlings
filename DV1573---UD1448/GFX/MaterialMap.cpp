#include "Pch/Pch.h"
#include "MaterialMap.h"

MaterialMap* MaterialMap::m_materialMapInstance = 0;

MaterialMap::MaterialMap() {}

MaterialMap* MaterialMap::getInstance()
{
	if (m_materialMapInstance == 0) {
		m_materialMapInstance = new MaterialMap();
	}
	return m_materialMapInstance;
}

void MaterialMap::cleanUp()
{
	std::map<std::string, Materials*>::iterator it;

	for (it = m_Materials.begin(); it != m_Materials.end(); it++) {
		delete it->second;
	}

	m_Materials.clear();
}

bool MaterialMap::existsWithName(std::string name)
{
	if (m_Materials.find(name) != m_Materials.end()) {
		return true;
	}
	return false;
}

void MaterialMap::reload()
{
	//std::map<std::string, Shader*>::iterator it;

	//for (it = m_Materials.begin(); it != m_Materials.end(); it++) {
	//	Materials* tempMaterial = new Materials(it->second->getShaderNames()[0], it->second->getShaderNames()[1]);
	//	if (tempMaterial->getValid()) {
	//		delete it->second;
	//		it->second = tempShader;

	//		logInfo("Recompile Shader Completed");
	//	}
	//	else {
	//		delete tempShader;
	//	}
	//}

	//for (it = m_shaderMap.begin(); it != m_shaderMap.end(); it++) {
	//	logTrace("Shader ID: {0}", it->second->getShaderID());
	//}
}

void MaterialMap::useByName(std::string name)
{
	//if (existsWithName(name)) {
	//	m_Materials[name]->use();
	//}
}

Materials* MaterialMap::getMaterial(std::string name)
{
	if (existsWithName(name))
	{
		return m_Materials[name];
	}

	return nullptr;
}

void MaterialMap::destroy()
{
	cleanUp();
	delete m_materialMapInstance;
}
