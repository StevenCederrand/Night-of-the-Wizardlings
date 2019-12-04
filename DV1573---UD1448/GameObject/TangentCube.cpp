#include "Pch/Pch.h"
#include "TangentCube.h"
#include "Texture/stb_image.h"

TangentCube::TangentCube()
{
	m_type = NORMAL_MAPPED;



}

TangentCube::~TangentCube()
{
}

void TangentCube::loadTexture()
{
	glGenTextures(1, &m_normalMap);
	glBindTexture(GL_TEXTURE_2D, m_normalMap);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrOfChannels;
	std::string normalMapPath = "Assets/Textures/NormalMap/BricksNRM.jpg";
	unsigned char* data = stbi_load(normalMapPath.c_str(), &width, &height, &nrOfChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		logTrace("Normal map texture failed to load");
	}
	stbi_image_free(data);	
}

const GLuint& TangentCube::getNormalMapTexture() const
{
	return m_normalMap;
}

void TangentCube::update(float dt)
{
}
