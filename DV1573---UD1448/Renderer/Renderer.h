#ifndef _RENDERER_h
#define _RENDERER_h

//Define the names of the shaders
#define LIGHT_CULL "Light_Cull"
#define BASIC_FORWARD "Basic_Forward"
#define DEPTH_MAP "Depth_Map"
#define SKYBOX "Skybox_Shader"
#define ANIMATION "Basic_Animation"
#define FRESNEL "Fresnel_Shader"
#define ENEMYSHIELD "Enemy_Shield"
#define TRANSPARENT "Transparent_Render"
#define SKYBOX "Skybox_Shader"
#define HUD "Hud_Shader"
#define WHUD "wHudShader"
#define PARTICLES "Particle_Shader"
#define SSAO_RAW "SSAO_Compute"
#define NAIVE_BLUR "NaiveBlur"
#define V_BLUR "VerticalBlur"
#define H_BLUR "HorizontalBlur"
//Rendering Options 
#define FORWARDPLUS false;
#define SSAO false; 
#define N_BLUR false; //Use naive blur?

//Number of SSAO kernels allowed in the system
#define SSAO_KERNELS 32
//Max number of lights
#define P_LIGHT_COUNT 64


#pragma region Includes
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <GameObject/AnimatedObject.h>
#include <Mesh/MeshFormat.h>
#include <Spells/SpellHandler.h>
#include <Renderer/SkyBox.h>
#include <System/Timer.h>
#include <Renderer/BloomBlur.h>
#include <Spells/SpellHandler.h>
#include <HUD/HudObject.h>
#include <HUD/WorldHudObject.h>
#include "NotificationStructure.h"
#include <Text/FreeType.h>
#include <GameObject/ShieldObject.h>
#include <GameObject/EnemyShieldObject.h>
#include <GFX/Pointlight.h>
#include <Particles/Particles.h>
#include <Particles/ParticleBuffers.h>
#pragma endregion

#include <System/MemoryUsage.h>

#define P_LIGHT_COUNT 64
#define P_LIGHT_RADIUS 5

struct ObjectRenderData {
	Buffers buffer;
	glm::vec3 worldPos;
};

struct LightIndex {
	int index[P_LIGHT_COUNT];
};

struct PLIGHT {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec4 attenAndRadius;
	float strength;
	int index;
};

enum RENDER_TYPE {
	STATIC,
	DYNAMIC,
	ANIMATEDSTATIC,
	ANIMATEDDYNAMIC,
	SPELL,
	PICKUP,
	SHIELD,
	FIRESPELL,
	POINTLIGHT_SOURCE,
	ENEMY_SHIELD,
	SKYOBJECTS
};

class Renderer
{
private:
	std::vector<NotificationText> m_bigNotifications;
	std::vector<NotificationText> m_killFeed;
	std::vector<NotificationText> m_killNotification;

private:
	static Renderer* m_rendererInstance;
	GLFWwindow* m_gWindow;
	Camera* m_camera;
	FreeType* m_text;
	SkyBox* m_skyBox;
	Timer m_timer;
	SpellHandler* m_spellHandler;

	

	//Store gameobjects directly to the renderer
	std::vector<GameObject*> m_staticObjects;
	std::vector<GameObject*> m_dynamicObjects;
	std::vector<GameObject*> m_anistaticObjects;
	std::vector<GameObject*> m_anidynamicObjects;
	std::vector<GameObject*> m_spells;
	std::vector<PLIGHT> m_lights;
	std::vector<GameObject*> m_pickups;
	std::vector<GameObject*> m_enemyShieldObject;
	std::vector<GameObject*> m_skyObjects;
	GameObject* m_shieldObject;

	std::unordered_map<GLuint, std::vector<HudObject*>> m_2DHudMap;
	std::unordered_map<GLuint, std::vector<WorldHudObject*>> m_worldHudMap;

	//Buffers
	unsigned int m_depthFBO;
	unsigned int m_depthMap;

#if SSAO
	std::vector<glm::vec3> m_SSAOKernels;
	std::vector<glm::vec3> m_SSAONoise;

	unsigned int m_SSAOFBO;
	unsigned int m_SSAOColourBuffer;
	unsigned int m_SSAONoiseTexture;	//SSAO noise texture
#endif

	//unsigned int m_hdrFbo;
	unsigned int m_colourBuffer;
	unsigned int m_rbo;
	bool m_renderedDepthmap;

	//Storage Buffer for light indecies
	unsigned int m_lightIndexSSBO;
	glm::uvec2 workGroups;
	void renderAndAnimateNetworkingTexts();


#pragma region Particles
	//Particle variables
	unsigned int m_matrixID;
	unsigned int m_cameraID;
	unsigned int m_sizeID;
	unsigned int m_glowID;
	unsigned int m_scaleDirection;
	unsigned int m_swirl;
	unsigned int m_fadeID;
	unsigned int m_colorID;
	unsigned int m_blendColorID;

	TextureInfo m_txtInfo;
	PSinfo m_PSinfo;
	ParticleBuffers* deathBuffer;

	std::vector<ParticleSystem> m_particleSystems;
	void renderBigNotifications();
	void renderKillFeed();
	void createDepthMap();
	void initShaders();
	void bindMatrixes(const std::string& shaderName);
	void bindMatrixes(Shader* shader);
	//BloomBlur* m_bloom;
	//SpellHandler* m_spellHandler;

	Renderer();
	~Renderer();
public:
	static Renderer* getInstance();

	void init(GLFWwindow* window);
	void setupCamera(Camera* camera);

	void destroy();
	void clear();
	//SUBMIT POINTLIGHTS BY IN THEM HERE
	void submit(GameObject* gameObject, RENDER_TYPE objType);
	void submit2DHUD(HudObject* hud);
	void submitWorldHud(WorldHudObject* wHud);
	void submitSkybox(SkyBox* skybox);
	void submitSpellhandler(SpellHandler* spellhandler);
	void removeRenderObject(GameObject* gameObject, RENDER_TYPE objType); //Remove an object from the dynamic array
	void renderSkybox();
	void render();
	void renderDepthmap(); //Generate a depthmap, this is used for both Forward+ and SSAO
	void renderHUD();
	void renderWorldHud();
	
	void addBigNotification(NotificationText notification);
	void addKillFeed(NotificationText notification);
	void addKillNotification(NotificationText notification);
	unsigned int getTextWidth(const std::string& text, const glm::vec3& scale);

	void renderSpell(SpellHandler* spellHandler);
	Camera* getMainCamera() const;

	void initializeParticle();
	void updateParticles(float dt);
	void removePoof();
	void death();
};

#endif
