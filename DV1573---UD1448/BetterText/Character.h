#ifndef _CHARACTER_H
#define _CHARACTER_H
#include <Pch/Pch.h>

struct Character {
	int id = -1;
	glm::vec4 textureCoord = glm::vec4(0.0f);
	glm::vec2 offset = glm::vec2(0.0f);
	glm::vec2 size = glm::vec2(0.0f);
	float xAdvance = 0.0f;
};


#endif
