#pragma once
#include <Pch/Pch.h>
#include <GameObject/Cube.h>


class Renderer
{
private:
	GLFWwindow* m_gWindow;
	Camera* m_camera;
	//Buffer Object
	GLuint m_VBO;

	unsigned int m_Fbo;
	unsigned int m_FboAttachments[2];

	static Renderer* m_rendererInstance;
	Renderer();
public:


	void update(float dt);
	static Renderer* getInstance();
	void init(GLFWwindow* window);
	
	void destroy();
	const GLuint& getVBO() const;
	void render(Cube* cube);

	Camera* getMainCamera() const;

};

