#include <Pch/Pch.h>
#include "FontType.h"

FontType::FontType(const std::string& fontTexture, const std::string& fontFile)
{	
	loadTexture(fontTexture);
	m_textMeshCreator = new TextMeshCreator(fontFile);
}

FontType::~FontType()
{
	delete m_textMeshCreator;
}

const unsigned int& FontType::getTextureAtlas() const
{
	return m_textureAtlas;
}

TextMeshData FontType::loadText(GUIText* text)
{
	return m_textMeshCreator->createTextMesh(text);
}

bool FontType::loadTexture(const std::string& fontTexture)
{
	glGenTextures(1, &m_textureAtlas);
	glBindTexture(GL_TEXTURE_2D, m_textureAtlas);

	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(fontTexture.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
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
		
	}
	else
	{
		logError("(FontType.cpp) Failed to load texture {0}", fontTexture.c_str());
		return false;
	}

	stbi_image_free(data);

	return true;
}
