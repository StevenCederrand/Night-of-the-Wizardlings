#pragma once
#include <Pch/Pch.h>
#include <GameObject/Cube.h>
#include <Mesh/MeshFormat.h>

class Renderer
{
private:
	GLFWwindow* m_gWindow;
	Camera* m_camera;

	unsigned int m_Fbo;
	unsigned int m_FboAttachments[2];

	static Renderer* m_rendererInstance;
	Renderer();
public:


	void update(float dt);
	static Renderer* getInstance();
	void init(GLFWwindow* window);
	
	void destroy();
	void render(Cube* cube);
	void render(Buffers buffer, glm::vec3 worldPos);


	Camera* getMainCamera() const;
};

