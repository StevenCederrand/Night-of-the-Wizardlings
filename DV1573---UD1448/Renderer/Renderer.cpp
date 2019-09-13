#include <Pch/Pch.h>
#include "Renderer.h"

Renderer* Renderer::m_rendererInstance = 0;

Renderer::Renderer()
{
	m_gWindow = nullptr;
	m_camera = nullptr;
 	glGenBuffers(1, &m_VBO); //Generate a VBO
	glEnable(GL_DEPTH_TEST);


	//Enable this when more objects are being loaded!
	/*glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);*/
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
	glDeleteBuffers(1, &m_VBO);
	delete m_camera;
	delete m_rendererInstance;
}	

void Renderer::update(float dt) {
	m_camera->fpsControls(dt);
}

void Renderer::render(Cube* cube) {
	m_camera->update(m_gWindow);
	
	ShaderMap::getInstance()->useByName("Basic_Forward");	

	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("viewMatrix", m_camera->getViewMat());
	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("projectionMatrix", m_camera->getProjMat());


	glBindVertexArray(cube->getVAO());

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, cube->getWorldPos());

	ShaderMap::getInstance()->getShader("Basic_Forward")->setMat4("modelMatrix", model);
	
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}


const GLuint& Renderer::getVBO() const{
	return m_VBO;
}


Camera* Renderer::getMainCamera() const
{
	return m_camera;
}
