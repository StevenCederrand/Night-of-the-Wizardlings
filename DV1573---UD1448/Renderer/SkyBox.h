#pragma once
#include <Pch/Pch.h>

struct SkyBuffer {
	GLuint VAO;
	GLuint VBO;
};

class SkyBox
{
private:
	//Order of the texture are: Right, Left, Top, Bottom, Front and Back.
	std::vector<std::string> faces
	{
	"Assets/Textures/Skybox/Daylight Box_Right.bmp",
	"Assets/Textures/Skybox/Daylight Box_Left.bmp",
	"Assets/Textures/Skybox/Daylight Box_Top.bmp",
	"Assets/Textures/Skybox/Daylight Box_Bottom.bmp",
	"Assets/Textures/Skybox/Daylight Box_Front.bmp",
	"Assets/Textures/Skybox/Daylight Box_Back.bmp"
	};

	unsigned int cubemapTexture;
	SkyBuffer sky_Buffer;



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
};