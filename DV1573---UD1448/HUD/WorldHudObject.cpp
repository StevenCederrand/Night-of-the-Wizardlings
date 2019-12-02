#include <Pch/Pch.h>
#include "WorldHudObject.h"

WorldHudObject::WorldHudObject(const std::string& texturePath, const glm::vec3& center, const glm::vec2& scale)
{
	loadTexture(texturePath);
	setupBuffers();

	m_center = center;
	m_scale = scale;
	m_alpha = 1.0f;
	m_xClip = 1.0f;
	m_yClip = 1.0f;
	m_shouldRender = true;
	
}

WorldHudObject::~WorldHudObject()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void WorldHudObject::setCenter(const glm::vec3& center)
{
	m_center = center;
}

void WorldHudObject::setScale(const glm::vec2& scale)
{
	m_scale = scale;
}

void WorldHudObject::setAlpha(const float& alpha)
{
	m_alpha = alpha;
}

void WorldHudObject::setXClip(const float& xClip)
{
	m_xClip = xClip;
}

void WorldHudObject::setYClip(const float& yClip)
{
	m_yClip = yClip;
}

void WorldHudObject::setShouldRender(bool condition)
{
	m_shouldRender = condition;
}

const GLuint& WorldHudObject::getVAO() const
{
	return m_vao;
}

const GLuint& WorldHudObject::getTextureID() const
{
	return m_textureID;
}

const float& WorldHudObject::getAlpha() const
{
	return m_alpha;
}

const float& WorldHudObject::getXClip() const
{
	return m_xClip;
}

const float& WorldHudObject::getYClip() const
{
	return m_yClip;
}

const glm::vec3& WorldHudObject::getCenter() const
{
	return m_center;
}

const glm::vec2& WorldHudObject::getScale() const
{
	return m_scale;
}

const bool& WorldHudObject::getShouldRender() const
{
	return m_shouldRender;
}

void WorldHudObject::setupBuffers()
{
	// setup plane VAO
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexData), &m_vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void WorldHudObject::loadTexture(const std::string& texturePath)
{
	auto* tmap = HudTextureMap::getInstance();
	const GLuint* tID = tmap->getTextureID(texturePath);
	if (tID != nullptr) {
		m_textureID = *tID;
		return;
	}

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
	if (data)
	{
		GLenum format = {};

		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);

		glGenerateMipmap(GL_TEXTURE_2D);
		tmap->addTextureID(texturePath, m_textureID);
	}
	else
	{
		logError("(HudObject.cpp) Failed to load texture {0}", texturePath.c_str());
	}

	stbi_image_free(data);
}
