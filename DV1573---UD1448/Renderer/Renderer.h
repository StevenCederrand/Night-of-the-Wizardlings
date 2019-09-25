#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Mesh/MeshFormat.h>
#include <Renderer/SkyBox.h>

struct ObjectRenderData {
	Buffers buffer;
	glm::vec3 worldPos;
};


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
	void setupCamera(Camera* camera);

	void destroy();
	void bindMatrixes(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);
	void renderSkybox(const SkyBox& skybox);
	void render(const GameObject& gameObject);
	void render(const GameObject& gameObject, int meshIndex);
	void render(Buffers buffer, glm::vec3 worldPos);

	Camera* getMainCamera() const;
};

