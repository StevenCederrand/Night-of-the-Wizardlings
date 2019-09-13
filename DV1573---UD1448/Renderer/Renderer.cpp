#include <Pch/Pch.h>
#include "Renderer.h"

Renderer* Renderer::m_rendererInstance = 0;

Renderer::Renderer()
{
	m_gWindow = nullptr;
	m_camera = nullptr;
	glGenBuffers(1, &m_VBO); //Generate a VBO

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
	//initBasicQuad();
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

void Renderer::render(Cube* cube) {
	ShaderMap* shaderMap = shaderMap->getInstance();
	shaderMap->useByName("Basic_Forward");


	glBindVertexArray(cube->getVAO());

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, cube->getWorldPos());

	shaderMap->getShader("Basic_Forward")->setMat4("modelMatrix", model);


	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}


GLuint Renderer::getVBO() const {
	return m_VBO;
}

