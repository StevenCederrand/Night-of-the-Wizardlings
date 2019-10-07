#ifndef _RENDERER_h
#define _RENDERER_h

#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Mesh/MeshFormat.h>
#include <Renderer/SkyBox.h>

#define P_LIGHT_COUNT 10

struct ObjectRenderData {
	Buffers buffer;
	glm::vec3 worldPos;
};

struct Pointlight {
	glm::vec3 position; //EXPAND ONE OF THESE VEC3's TO ALSO INCLUDE THE RADIUS!!!
	glm::vec3 attenuation; //EXPAND ONE OF THESE VEC3's TO ALSO INCLUDE THE RADIUS!!!
	float radius;
};

struct LightIndex {
	int index[P_LIGHT_COUNT];
};

enum ObjectType {
	STATIC,
	DYNAMIC
};

class Renderer
{
private:
	static Renderer* m_rendererInstance;
	GLFWwindow* m_gWindow;
	Camera* m_camera;

	//Store gameobjects directly to the renderer
	std::vector<GameObject*> m_staticObjects;
	std::vector<GameObject*> m_dynamicObjects;

	//Buffers
	unsigned int m_depthFBO;
	unsigned int m_depthMap;
	unsigned int m_hdrFbo;
	unsigned int m_colourBuffer;
	unsigned int m_rbo;

	//Storage Buffer for light indecies
	unsigned int m_lightIndexSSBO;
	
	glm::vec2 workGroups;
	std::vector<Pointlight> m_pLights;

	
	void createDepthMap();
	void initShaders();
	void bindMatrixes(const std::string& shaderName);
	
	void renderDepth(const GameObject& gameObject, const int& meshIndex);
	
	void lightCull();

	void renderColor(const GameObject& gameObject, const int& meshIndex);

	Renderer();
public:


	void update(float dt);
	static Renderer* getInstance();
	
	void init(GLFWwindow* window);
	void setupCamera(Camera* camera);

	void destroy();
	void submit(GameObject* gameObject, ObjectType objType);
	void bindMatrixes(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);
	void renderSkybox(const SkyBox& skybox);
	void render(const GameObject& gameObject, int meshIndex);
	void render();
	Camera* getMainCamera() const;
};

#endif