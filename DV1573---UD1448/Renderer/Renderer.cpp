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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	m_gWindow = window;
}
void Renderer::setupCamera(Camera* camera)
{
	if (camera == nullptr) {
		return;
	}
	m_camera = camera;
}

void Renderer::destroy()
{
	delete m_rendererInstance;
}
void Renderer::bindMatrixes(const glm::mat4& viewMatrix, const glm::mat4& projMatrix)
{
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("viewMatrix", m_camera->getViewMat());
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("projectionMatrix", m_camera->getProjMat());
}

void Renderer::update(float dt) {
	m_camera->fpsControls(dt);
	m_camera->update(m_gWindow);
}

void Renderer::render(const GameObject& gameObject) {

	glBindVertexArray(gameObject.getMesh()->getBuffers().vao);

	//Apply transformation
	glm::mat4 worldMatrix = glm::mat4(1.0f);
	const Transform& meshTransform = gameObject.getTransform();
	worldMatrix = glm::translate(worldMatrix, meshTransform.position);
	worldMatrix = glm::scale(worldMatrix, meshTransform.scale);
	worldMatrix *= glm::mat4_cast(meshTransform.rotation);

	//Set matrices
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("viewMatrix", m_camera->getViewMat());
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("projectionMatrix", m_camera->getProjMat());
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("modelMatrix", worldMatrix);

	//Drawcall
	glDrawElements(GL_TRIANGLES, gameObject.getMesh()->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);

}

void Renderer::render(const GameObject& gameObject, int meshIndex) {

	glBindVertexArray(gameObject.getMesh(meshIndex)->getBuffers().vao);

	//Apply transformation
	glm::mat4 worldMatrix = glm::mat4(1.0f);
	Transform meshTransform = gameObject.getTransform(meshIndex);
	worldMatrix = glm::translate(worldMatrix, meshTransform.position);
	worldMatrix = glm::scale(worldMatrix, meshTransform.scale);
	worldMatrix *= glm::mat4_cast(meshTransform.rotation);

	//Set matrices
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("viewMatrix", m_camera->getViewMat());
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("projectionMatrix", m_camera->getProjMat());
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("modelMatrix", worldMatrix);

	//Drawcall
	glDrawElements(GL_TRIANGLES, gameObject.getMesh(meshIndex)->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);

}


void Renderer::render(Buffers buffer, glm::vec3 worldPos) {
	

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
