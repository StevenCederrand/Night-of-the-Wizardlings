#pragma once
#include <Pch/Pch.h>

struct Transform {
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	Transform()
	{
		position = glm::vec3();
		rotation = glm::quat();
		scale = glm::vec3(1.0f);
	}
};

struct Vertices
{
	glm::vec3 position;
	glm::vec2 UV;
	glm::vec3 Normals;
};

struct Face
{
	int indices[3];
};

struct Material
{
	std::string name;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	std::vector<GLuint> textureID;
};

struct Buffers
{
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
	int nrOfFaces;
};