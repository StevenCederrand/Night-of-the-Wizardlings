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
	//TODO: Also being done in render()
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("viewMatrix", m_camera->getViewMat());
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("projectionMatrix", m_camera->getProjMat());
}

void Renderer::renderSkybox(const SkyBox& skybox)
{
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	//glDepthMask(false);
	ShaderMap::getInstance()->useByName("Skybox_Shader");
	ShaderMap::getInstance()->getShader("Skybox_Shader")->setMat4("viewMatrix", glm::mat4(glm::mat3(m_camera->getViewMat())));
	ShaderMap::getInstance()->getShader("Skybox_Shader")->setMat4("projectionMatrix", m_camera->getProjMat());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getCubeMapTexture());
	glBindVertexArray(skybox.getVAO());
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	glBindVertexArray(0);
	//glDepthMask(true);
	glEnable(GL_CULL_FACE);
}

void Renderer::update(float dt) {
	m_camera->fpsControls(dt);
	m_camera->update(m_gWindow);
}

void Renderer::render(const GameObject& gameObject) {
	Mesh* meshRef = MeshMap::getInstance()->getMesh(gameObject.getMeshN(0));
	const Transform meshTransform = gameObject.getTransform();

	glBindVertexArray(meshRef->getBuffers().vao);

	//Apply transformation
	//TODO: Should the gameobject hold the worldmatrix?
	glm::mat4 worldMatrix = glm::mat4(1.0f);
	worldMatrix = glm::translate(worldMatrix, meshTransform.position);
	worldMatrix = glm::scale(worldMatrix, meshTransform.scale);
	worldMatrix *= glm::mat4_cast(meshTransform.rotation);

	//Set matrices TODO: function exists for this, evaluate what to keep
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("viewMatrix", m_camera->getViewMat());
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("projectionMatrix", m_camera->getProjMat());
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("modelMatrix", worldMatrix);

	//Drawcall

	glDrawElements(GL_TRIANGLES, meshRef->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);

}

void Renderer::render(const GameObject& gameObject, int meshIndex) {
	Mesh* meshRef = MeshMap::getInstance()->getMesh(gameObject.getMeshN(meshIndex));
	const Transform meshTransform = gameObject.getTransform(meshIndex);

	glBindVertexArray(meshRef->getBuffers().vao);

	//Apply transformation
	//TODO: Should the gameobject hold the worldmatrix? 
	// Yes it should
	glm::mat4 worldMatrix = glm::mat4(1.0f);
	worldMatrix = glm::translate(worldMatrix, meshTransform.position);
	worldMatrix = glm::scale(worldMatrix, meshTransform.scale);
	worldMatrix *= glm::mat4_cast(meshTransform.rotation);

	//Set matrices
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("viewMatrix", m_camera->getViewMat());
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("projectionMatrix", m_camera->getProjMat());
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("modelMatrix", worldMatrix);

	//Drawcall
	glDrawElements(GL_TRIANGLES, meshRef->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);

}


void Renderer::render(Buffers buffer, glm::vec3 worldPos) {
	
	//TODO: Remove?

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
