#include <Pch/Pch.h>
#include "Renderer.h"

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

Renderer::Renderer()
{
	m_gWindow = nullptr;
	m_camera = nullptr;
}

Renderer::Renderer(Camera* camera, GLFWwindow* window)
{
	m_gWindow = window;
	m_camera = camera;
	//Might be moved, have a callback to enable cursor if needed.
	
	glfwSetInputMode(m_gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	initBasicQuad();

}

Renderer::~Renderer()
{
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;
	
	lastX = xpos;
	lastY = ypos;
	
	logTrace(lastX);

	//mouseControls(xOffset, yOffset, true);
}