#pragma once
#include <Pch/Pch.h>


class Renderer
{
private:
	static GLFWwindow* m_gWindow;
	static Camera* m_camera;
	static unsigned int m_Fbo;
	static unsigned int m_FboAttachments[2];

	static glm::mat4 m_modelMat,
		m_viewMat,
		m_projMat;


	static GLuint m_rQuadVAO;
	static GLuint m_rQuadVBO;

	static float m_rQuadData[24];

	static Renderer* m_rendererInstance;
	Renderer();

public:

	static Renderer* getInstance();
	static void init(Camera* camera, GLFWwindow* window);
	static void destroy();

	static void initBasicQuad();
	static void drawQuad();
	static void render();

};

