#ifndef _CHARACTER_H
#define _CHARACTER_H
#include <Pch/Pch.h>

struct Character {

	int id = -1;
	glm::vec4 textureCoord;
	glm::vec2 offset;
	glm::vec2 size;
	float xAdvance;
};


#endif
