#ifndef _RENDERER_h
#define _RENDERER_h

//Define the names of the shaders
#define LIGHT_CULL "Light_Cull"
#define BASIC_FORWARD "Basic_Forward"
#define DEPTH_MAP "Depth_Map"
#define SKYBOX "Skybox_Shader"
#define ANIMATION "Basic_Animation"
#define DEBUG "Debug_Forward"
#define BLOOM "Bloom_Shader"
#define BLUR "Blur_Shader"
#define BLOOM_BLUR "BloomBlur_Shader"
#define HUD "Hud_Shader"


#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <GameObject/AnimatedObject.h>
#include <Mesh/MeshFormat.h>
#include <Spells/SpellHandler.h>
#include <Renderer/SkyBox.h>
#include <System/Timer.h>
#include <Renderer/BloomBlur.h>
#include <Spells/SpellHandler.h>
#include <Renderer/HudObject.h>
#include <Text/FreeType.h>

#define P_LIGHT_COUNT 64
#define P_LIGHT_RADIUS 2

struct ObjectRenderData {
	Buffers buffer;
	glm::vec3 worldPos;
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
	FreeType* m_text;
	SkyBox* m_skyBox;

	Timer m_timer;

	//Store gameobjects directly to the renderer
	std::vector<GameObject*> m_staticObjects;
	std::vector<GameObject*> m_dynamicObjects;
	std::vector<GameObject*> m_anistaticObjects;
	std::vector<GameObject*> m_anidynamicObjects;
	std::vector<GameObject*> m_spells; 

	std::unordered_map<GLuint, std::vector<HudObject*>> m_2DHudMap;

	//Buffers
	unsigned int m_depthFBO;
	unsigned int m_depthMap;
	unsigned int m_hdrFbo;
	unsigned int m_colourBuffer;
	unsigned int m_rbo;

	//Storage Buffer for light indecies
	unsigned int m_lightIndexSSBO;
	
	glm::uvec2 workGroups;

	void renderHUD();
	void createDepthMap();
	void initShaders();
	void bindMatrixes(const std::string& shaderName);
	
	
	BloomBlur* m_bloom;
	//SpellHandler* m_spellHandler;

	Renderer();
	~Renderer();
public:


	static Renderer* getInstance();
	
	void init(GLFWwindow* window);
	void setupCamera(Camera* camera);

	void destroy();
	void clear();
	void submit(GameObject* gameObject, ObjectType objType);
	void submit2DHUD(HudObject* hud);
	void removeDynamic(GameObject* gameObject, ObjectType objType); //Remove an object from the dynamic array
	void renderSkybox(SkyBox* skybox);
	void render(SkyBox* m_skybox, SpellHandler* m_spellHandler);
	//void renderSpell();
	void renderDebug();

	void renderSpell(SpellHandler* spellHandler);
	Camera* getMainCamera() const;

};

#endif