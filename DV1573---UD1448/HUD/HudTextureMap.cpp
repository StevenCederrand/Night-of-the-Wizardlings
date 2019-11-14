#include "Pch/Pch.h"
#include "HudTextureMap.h"

HudTextureMap::HudTextureMap(){}
HudTextureMap::~HudTextureMap(){}

void HudTextureMap::destroy()
{
	for (auto item : m_textureMap) {
		glDeleteTextures(1, &item.second);
	}

	m_textureMap.clear();
}

HudTextureMap* HudTextureMap::getInstance()
{
	static HudTextureMap instance;
	return &instance;
}

void HudTextureMap::addTextureID(const std::string& key, const GLuint& textureID)
{
	auto item = m_textureMap.find(key);

	if (item == m_textureMap.end()) {
		m_textureMap[key] = textureID;
	}
}

const GLuint* HudTextureMap::getTextureID(const std::string& key) const
{
	auto item = m_textureMap.find(key);

	if (item != m_textureMap.end()) {
		return &item._Ptr->_Myval.second;
	}

	return nullptr;
}
