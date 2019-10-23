#include <Pch/Pch.h>
#include <Texture/stb_image.h>
#include "SkyBox.h"

SkyBox::SkyBox()
{
	cubemapTexture = createCubeMap(faces);
}

SkyBox::~SkyBox()
{
}

unsigned int SkyBox::createCubeMap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	int width, height, nrOfChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrOfChannels, STBI_rgb_alpha);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
			logError("Loading Texture Failed");
	}
	return textureID;
}

const GLuint& SkyBox::getVAO() const
{
	return sky_Buffer.VAO;
}

unsigned int SkyBox::getCubeMapTexture() const
{
	return cubemapTexture;
}

void SkyBox::prepareBuffers()
{ 
	glGenVertexArrays(1, &sky_Buffer.VAO);
	glGenBuffers(1, &sky_Buffer.VBO);

	glBindVertexArray(sky_Buffer.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, sky_Buffer.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
}
