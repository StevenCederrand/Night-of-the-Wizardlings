#include <Pch/Pch.h>
#include "Renderer.h"


Renderer* Renderer::m_rendererInstance = 0;

Renderer::Renderer()
{
	m_gWindow = nullptr;
	m_camera = nullptr;


	glEnable(GL_DEPTH_TEST);
	//Enable this when more objects are being loaded!
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

Renderer* Renderer::getInstance()
{
	if (m_rendererInstance == 0) {
		m_rendererInstance = new Renderer();
	}
	return m_rendererInstance;
}

void Renderer::init(GLFWwindow* window)
{
	m_camera = new Camera();
	m_gWindow = window;
}	

void Renderer::destroy()
{
	delete m_camera;
	delete m_rendererInstance;
}	

void Renderer::update(float dt) {
	m_camera->fpsControls(dt);
}

void Renderer::render(const GameObject& gameObject) {
	m_camera->update(m_gWindow);

	ShaderMap::getInstance()->useByName("Basic_Forward");

	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("viewMatrix", m_camera->getViewMat());
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("projectionMatrix", m_camera->getProjMat());

	glBindVertexArray(gameObject.getMesh()->getBuffers().vao);

	glm::mat4 worldMatrix = glm::mat4(1.0f);
	worldMatrix = glm::translate(worldMatrix, gameObject.getTransform().m_worldPos);

	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("modelMatrix", worldMatrix);

	glDrawElements(GL_TRIANGLES, gameObject.getMesh()->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);

}

void Renderer::render(Buffers buffer, glm::vec3 worldPos) {
	m_camera->update(m_gWindow);

	ShaderMap::getInstance()->useByName("Basic_Forward");

	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("viewMatrix", m_camera->getViewMat());
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("projectionMatrix", m_camera->getProjMat());

	glBindVertexArray(buffer.vao);

	glm::mat4 worldMatrix = glm::mat4(1.0f);
	worldMatrix = glm::translate(worldMatrix, worldPos);

	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("modelMatrix", worldMatrix);
	
	glDrawElements(GL_TRIANGLES, buffer.nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);
}

Camera* Renderer::getMainCamera() const
{
	return m_camera;
}
