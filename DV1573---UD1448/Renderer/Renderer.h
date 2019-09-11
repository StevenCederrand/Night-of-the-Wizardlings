#pragma once
#include <Pch/Pch.h>

class Renderer
{
private:
	GLFWwindow* gWindow;

	unsigned int m_Fbo;
	unsigned int m_FboAttachments[2];

	glm::mat4 modelMat,
			  viewMat,
			  projMat;

public:

};

