#include <Pch/Pch.h>
#include "Renderer.h"

Renderer* Renderer::m_rendererInstance = 0;

GLFWwindow* Renderer::m_gWindow;
Camera* Renderer::m_camera;
unsigned int Renderer::m_Fbo;
unsigned int Renderer::m_FboAttachments[2];

glm::mat4 Renderer::m_modelMat;
glm::mat4 Renderer::m_viewMat;
glm::mat4 Renderer::m_projMat;

//Temporary 
GLuint Renderer::m_rQuadVAO;
GLuint Renderer::m_rQuadVBO;

float Renderer::m_rQuadData[24] = {
	//VP			UV
	-0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.0f, 0.0f,
	0.5f, -0.5f,  1.0f, 0.0f,

	-0.5f,  0.5f,  0.0f, 1.0f,
	0.5f, -0.5f,  1.0f, 0.0f,
	0.5f,  0.5f,  1.0f, 1.0f
};

Renderer::Renderer()
{
	m_gWindow = nullptr;
	m_camera = nullptr;
}

Renderer* Renderer::getInstance()
{
	if (m_rendererInstance == 0) {
		m_rendererInstance = new Renderer();
	}
	return m_rendererInstance;
}

void Renderer::init(Camera* camera, GLFWwindow* window)
{
	m_camera = camera;
	m_gWindow = window;
	initBasicQuad();
}

void Renderer::destroy()
{
	delete m_rendererInstance;
}

void Renderer::initBasicQuad()
{
	glGenVertexArrays(1, &m_rQuadVAO);
	glGenBuffers(1, &m_rQuadVBO);

	glBindVertexArray(m_rQuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_rQuadVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(m_rQuadData), &m_rQuadData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
 	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void Renderer::drawQuad() {
	glBindVertexArray(m_rQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Renderer::render() {

	drawQuad();
}