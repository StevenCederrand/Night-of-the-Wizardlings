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

	glm::ivec4 bone;
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

	bool texture;
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
		struct skTransform
		{
			int jointid;
			Transform transform;
		};
		int id;
		std::vector<skTransform> local_joint_t;
	};

	// Standards
	std::string name;
	int keyframeFirst;
	int keyframeLast;
	float duration;
	float rate;
	std::vector<skKeyframe> keyframes;

};

struct BonePalleteBuffer
{
	glm::mat4 bones[64]{ glm::mat4() };
};

struct psBuffers
{
	unsigned int texture;
	unsigned int vao;
	unsigned int vertexBuffer;
	unsigned int directionalBuffer;
	unsigned int lifetimeBuffer;
	unsigned int shader;
	unsigned int vertexPosition;
	unsigned int vertexDirection;
	unsigned int vertexLife;
};