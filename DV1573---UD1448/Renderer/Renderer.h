#pragma once
#include <Pch/Pch.h>
#include <GameObject/Cube.h>


class Renderer
{
private:
	GLFWwindow* m_gWindow;
	Camera* m_camera;
	unsigned int m_Fbo;
	unsigned int m_FboAttachments[2];
	

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

	static Renderer* m_rendererInstance;
	Renderer();
public:
	void update(float dt);
	GLuint m_VBO;
	static Renderer* getInstance();
	void init(GLFWwindow* window);
	void destroy();

	GLuint getVBO();

	void initBasicQuad();
	void drawQuad();
	void render(Cube* cube);


	Camera* getMainCamera() const;

	GLuint createTexture(std::string path);
};

