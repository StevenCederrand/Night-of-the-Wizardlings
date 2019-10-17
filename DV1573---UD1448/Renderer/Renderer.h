#ifndef _RENDERER_h
#define _RENDERER_h

//Define the names of the shaders
#define LIGHT_CULL "Light_Cull"
#define BASIC_FORWARD "Basic_Forward"
#define DEPTH_MAP "Depth_Map"
#define SKYBOX "Skybox_Shader"
#define ANIMATION "Basic_Animation"

#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <GameObject/AnimatedObject.h>
#include <Mesh/MeshFormat.h>
#include <Spells/Spell.h>
#include <Spells/Spells.h>
#include <Renderer/SkyBox.h>
#include <System/Timer.h>

#define P_LIGHT_COUNT 64

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
	DYNAMIC,
	ANIMATEDSTATIC,
	ANIMATEDDYNAMIC,
	SPELL
};

class Renderer
{
private:
	static Renderer* m_rendererInstance;
	GLFWwindow* m_gWindow;
	Camera* m_camera;

	Timer m_timer;

	//Store gameobjects directly to the renderer
	std::vector<GameObject*> m_staticObjects;
	std::vector<GameObject*> m_dynamicObjects;
	std::vector<GameObject*> m_anistaticObjects;
	std::vector<GameObject*> m_anidynamicObjects;
	std::vector<GameObject*> m_spells; 
	
	//Buffers
	unsigned int m_depthFBO;
	unsigned int m_depthMap;
	unsigned int m_hdrFbo;
	unsigned int m_colourBuffer;
	unsigned int m_rbo;

	//Storage Buffer for light indecies
	unsigned int m_lightIndexSSBO;
	
	glm::uvec2 workGroups;
	std::vector<Pointlight> m_pLights;//The size of the vector is the number of lights
	
	void createDepthMap();
	void initShaders();
	void bindMatrixes(const std::string& shaderName);
	
	
	
	Renderer();
public:


	void update(float dt);
	static Renderer* getInstance();
	
	void init(GLFWwindow* window);
	void setupCamera(Camera* camera);

	void destroy();
	void submit(GameObject* gameObject, ObjectType objType);
	
	void removeDynamic(GameObject* gameObject); //Remove an object from the dynamic array
	void renderSkybox(const SkyBox& skybox);
	void render();

	void renderSpell(const AttackSpellBase* spellBase);
	Camera* getMainCamera() const;
};

#endif