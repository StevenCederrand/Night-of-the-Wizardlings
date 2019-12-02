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
//#define BLOOM "Bloom_Shader"
//#define BLUR "Blur_Shader"
//#define BLOOM_BLUR "BloomBlur_Shader"
#define HUD "Hud_Shader"
#define WHUD "wHudShader"
#define PARTICLES "Particle_Shader"

//Rendering Options 
#define FORWARDPLUS true;
#define SSAO true;

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
	ENEMY_SHIELD
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
	std::vector<GameObject*> m_shieldObject;
	std::vector<GameObject*> m_enemyShieldObject;

	std::unordered_map<GLuint, std::vector<HudObject*>> m_2DHudMap;
	std::unordered_map<GLuint, std::vector<WorldHudObject*>> m_worldHudMap;

	//Buffers
	unsigned int m_depthFBO;
	unsigned int m_depthMap;
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

	int	thisActive = 0;
	int	vertexCountDiff = 0;
	float emissionDiff = 0.0f;

	int	thisActive2 = 0;
	int	vertexCountDiff2 = 0;
	float emissionDiff2 = 0.0f;

	int	thisActive3 = 0;
	int	vertexCountDiff3 = 0;
	float emissionDiff3 = 0.0f;

	TextureInfo m_txtInfo;
	PSinfo m_PSinfo;
	PSinfo m_flameInfo;
	PSinfo m_enhanceInfo;
	PSinfo m_smoke;

	std::vector<ParticleSystem> m_particleSystems;
	//1 for every spelltype
	psBuffers attackBuffer;
	psBuffers flameBuffer; //Do I need 1 for every spell?
	psBuffers enhanceBuffer; // Yes, yes I do
	psBuffers smokeBuffer;

	ParticleBuffers* attackPS;
	ParticleBuffers* flamestrikePS;
	ParticleBuffers* enhancePS;
	ParticleBuffers* smokePS;
#pragma endregion

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
};

#endif
