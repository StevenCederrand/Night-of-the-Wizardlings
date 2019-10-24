#pragma once
#include <Pch/Pch.h>

struct SkyBuffer {
	GLuint VAO;
	GLuint VBO;
	GLuint CubemapTextureID;
};

class SkyBox
{
private:
	//Order of the texture are: Right, Left, Top, Bottom, Front and Back.
	std::vector<std::string> faces
	{
		"Assets/Textures/Night_Skybox/right.bmp",
		"Assets/Textures/Night_Skybox/left.bmp",
		"Assets/Textures/Night_Skybox/top.bmp",
		"Assets/Textures/Night_Skybox/bottom.bmp",
		"Assets/Textures/Night_Skybox/front.bmp",
		"Assets/Textures/Night_Skybox/back.bmp"
	};

	SkyBuffer m_buffer;



	glm::vec3 skyboxVertices[36] = {
		// positions          
		glm::vec3(-1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		 glm::vec3(1.0f, -1.0f, -1.0f),
		 glm::vec3(1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),

		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
		glm::vec3(-1.0f, -1.0f,  1.0f),

		 glm::vec3(1.0f, -1.0f, -1.0f),
		 glm::vec3(1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f,  1.0f,  1.0f),
		 glm::vec3(1.0f,  1.0f,  1.0f),
		 glm::vec3(1.0f,  1.0f, -1.0f),
		 glm::vec3(1.0f, -1.0f, -1.0f),

		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
		 glm::vec3(1.0f,  1.0f,  1.0f),
		 glm::vec3(1.0f,  1.0f,  1.0f),
		 glm::vec3(1.0f, -1.0f,  1.0f),
		glm::vec3(-1.0f, -1.0f,  1.0f),

		glm::vec3(-1.0f,  1.0f, -1.0f),
		 glm::vec3(1.0f,  1.0f, -1.0f),
		 glm::vec3(1.0f,  1.0f,  1.0f),
		 glm::vec3(1.0f,  1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),

		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f,  1.0f),
		 glm::vec3(1.0f, -1.0f, -1.0f),
		 glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f,  1.0f),
		 glm::vec3(1.0f, -1.0f,  1.0f)
	};

public:
	SkyBox();
	~SkyBox();

	unsigned int createCubeMap(std::vector<std::string> faces);
	const GLuint& getVAO() const;
	unsigned int getCubeMapTexture() const;
	void prepareBuffers();
	glm::mat4 getModelMatrix() const;
};