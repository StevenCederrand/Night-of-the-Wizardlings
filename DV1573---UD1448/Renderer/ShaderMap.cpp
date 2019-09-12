#include <Pch/Pch.h>
#include "ShaderMap.h"

std::map<std::string, Shader*> ShaderMap::m_shaderMap;

Shader* ShaderMap::createShader(std::string name, std::string vsName, std::string fsName)
{
	//If we have already reserved the name
	if (existsWithName(name)) {
		return nullptr;
	}

	Shader* shader = new Shader(vsName, fsName);
	m_shaderMap[name] = shader;
	return shader;
}

Shader* ShaderMap::getShader(std::string name)
{
	if (existsWithName(name)) 
	{
		return m_shaderMap[name];
	}

	return nullptr;
}

void ShaderMap::cleanUp()
{
	std::map<std::string, Shader*>::iterator it;

	for (it = m_shaderMap.begin(); it != m_shaderMap.end(); it++) {
		delete it->second;
	}

	m_shaderMap.clear();
}

bool ShaderMap::existsWithName(std::string name)
{
	if (m_shaderMap.find(name) != m_shaderMap.end()) {
		return true;
	}
	return false;
}

void ShaderMap::reload() {
	std::map<std::string, Shader*>::iterator it;

	for (it = m_shaderMap.begin(); it != m_shaderMap.end(); it++) {
		Shader* tempShader = new Shader(it->second->getShaderNames()[0], it->second->getShaderNames()[1]);
		delete it->second;
		it->second = tempShader;
	}

	for (it = m_shaderMap.begin(); it != m_shaderMap.end(); it++) {
		logTrace("Shader ID: {0}",it->second->getShaderID());
	}
}

void ShaderMap::useByName(std::string name) {

	if (existsWithName(name)) {
		m_shaderMap[name]->use();
	}
}