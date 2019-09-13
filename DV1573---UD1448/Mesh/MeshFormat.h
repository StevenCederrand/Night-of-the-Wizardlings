#pragma once
#include <Pch/Pch.h>

struct Verticex
{
	glm::vec3 position;
	glm::vec2 UV;
	glm::vec3 Normals;
};

struct Face
{
	int indices[3];
};