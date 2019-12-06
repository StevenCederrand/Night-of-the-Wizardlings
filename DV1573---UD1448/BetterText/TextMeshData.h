#ifndef _TEXT_MESH_DATA_H
#define _TEXT_MESH_DATA_H
#include <Pch/Pch.h>

struct TextMeshData {
	std::vector<glm::vec3> vertexPositions;
	std::vector<glm::vec2> textureCoords;
	float totalWordWith;
};


#endif