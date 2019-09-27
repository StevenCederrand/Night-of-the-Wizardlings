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

struct Vertex
{
	glm::vec3 position;
	glm::vec2 UV;
	glm::vec3 Normals;
};

struct Vertex2
{
	glm::vec3 position;
	glm::vec2 UV;
	glm::vec3 Normals;

	glm::vec4 bone;
	glm::vec4 weight;
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

struct Skeleton
{
	struct Joint
	{
		std::string name;
		int parentIndex;
		glm::mat4 invBindPose;
	};

	std::string name;
	std::vector<Joint> joints;

	Skeleton()
	{
		name = "";
	}

};

struct Animation
{
	// Skeleton animation
	struct skKeyframe
	{
		int id;
		std::vector<glm::vec3> local_joints_T;
		std::vector<glm::quat> local_joints_R;
		std::vector<glm::vec3> local_joints_S;
	};

	// Standards
	std::string name;
	int keyframeFirst;
	int keyframeLast;
	float duration;
	float rate;
	std::vector<skKeyframe> keyframes;


};