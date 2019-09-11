#pragma once
#include <Pch/Pch.h>

class Renderer
{
private:
	GLFWwindow* m_gWindow;
	Camera* m_camera;
	unsigned int m_Fbo;
	unsigned int m_FboAttachments[2];

	glm::mat4 m_modelMat,
			  m_viewMat,
			  m_projMat;


	GLuint m_rQuadVAO;
	GLuint m_rQuadVBO;
	
	float m_rQuadData[24] = {
		//VP			UV
		-0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  1.0f, 0.0f,

		-0.5f,  0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  1.0f, 1.0f
	};

public:
	Renderer();
	Renderer(Camera* camera, GLFWwindow* window);
	~Renderer();

	void initBasicQuad();
	void drawQuad();
	void render();

	

};

