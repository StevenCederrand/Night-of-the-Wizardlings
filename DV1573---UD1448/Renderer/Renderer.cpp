#include <Pch/Pch.h>
#include "Renderer.h"
#include <Texture/stb_image.h>

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

	createTexture("C:/Users/BTH/source/repos/Impwing/DV1573---UD1448/Assets/Textures");

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

GLuint Renderer::createTexture(std::string path)
{
	const char* filePath = path.c_str();

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrOfChannels;
	unsigned char* data = stbi_load(filePath, &width, &height, &nrOfChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		logError("Failed to load texture");
	stbi_image_free(data);

	return texture;
}
