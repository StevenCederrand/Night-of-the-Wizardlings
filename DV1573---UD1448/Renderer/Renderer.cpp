#include <Pch/Pch.h>
#include "Renderer.h"
#include <Networking/Client.h>
#include "TextRenderer.h"

#define TILE_SIZE 16

Renderer * Renderer::m_rendererInstance = 0;

Renderer::Renderer()
{
	m_gWindow = nullptr;
	m_camera = nullptr;
	glEnable(GL_MULTISAMPLE);
	createDepthMap();
	int x = -10;
	int z = -40;

	//Define Work Groups
	workGroups.x = (SCREEN_WIDTH + (SCREEN_WIDTH % TILE_SIZE)) / TILE_SIZE;
	workGroups.y = (SCREEN_HEIGHT + (SCREEN_HEIGHT % TILE_SIZE)) / TILE_SIZE;
	//INIT SHADER_STORAGE_BUFFER_OBJECT
	glGenBuffers(1, &m_lightIndexSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightIndexSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightIndex), 0, GL_STATIC_DRAW);
	//GL related calls
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//Initialize everything we need for SSAO
#if SSAO 

	//Generate SSAO Colour buffer
	glGenTextures(1, &m_SSAOColourBuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_SSAOColourBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindImageTexture(0, m_SSAOColourBuffer, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Framebuffer not complete!" << std::endl;

	//Generete SSAO kernels
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
	std::default_random_engine generator;

	for (int i = 0; i < SSAO_KERNELS; i++) {
		float scale = (float)i / (float)(SSAO_KERNELS);
		glm::vec3 v;
		v.x = 2.0f * (float)rand() / RAND_MAX - 1.0f;
		v.y = 2.0f * (float)rand() / RAND_MAX - 1.0f;
		v.z = 2.0f * (float)rand() / RAND_MAX - 1.0f;
		// Use an acceleration function so more points are
		// located closer to the origin
		v *= (0.1f + 0.9f * scale * scale);

		m_SSAOKernels.push_back(v);
	}

	//Generate a noise texture
	for (size_t i = 0; i < 256; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f);
		m_SSAONoise.push_back(noise);
	}

	glGenTextures(1, &m_SSAONoiseTexture);
	glBindTexture(GL_TEXTURE_2D, m_SSAONoiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 16, 16, 0, GL_RGB, GL_FLOAT, &m_SSAONoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

	printf("max global (total) work group size x:%i y:%i z:%i\n",
		work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);
	int work_grp_size[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
		work_grp_size[0], work_grp_size[1], work_grp_size[2]);
#endif

#if FORWARDPLUS
	createDepthMap();
#endif
	initShaders();

}

Renderer::~Renderer()
{
	delete m_text;

	if (deathBuffer)
		delete deathBuffer;


#if SSAO
	glDeleteTextures(1, &m_SSAOColourBuffer);
	glDeleteTextures(1, &m_SSAONoiseTexture);
	glDeleteFramebuffers(1, &m_SSAOFBO);

#endif

#if FORWARDPLUS
	glDeleteTextures(1, &m_depthMap);
	glDeleteFramebuffers(1, &m_depthFBO);
#endif
}

void Renderer::renderHUD()
{
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	// Get it ONCE instead of every iteration....
	auto* shader = ShaderMap::getInstance()->getShader(HUD);
	shader->use();
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	for (auto& item : m_2DHudMap) {

		auto& vec = item.second;

		if (vec.size() == 0)
			continue;

		auto* hudObjectDummy = vec[0];

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hudObjectDummy->getTextureID());

		for (size_t i = 0; i < vec.size(); i++)
		{

			auto* hudObject = vec[i];

			if (hudObject->getAlpha() == 0.0f)
				continue;

			shader->setMat4("modelMatrix", hudObject->getModelMatrix());
			shader->setFloat("alphaValue", hudObject->getAlpha());
			shader->setFloat("grayscale", hudObject->getGrayscale());
			shader->setVec2("clip", glm::vec2(hudObject->getXClip(), hudObject->getYClip()));
			glBindVertexArray(hudObject->getVAO());

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			glBindVertexArray(0);
		}

		glBindTexture(GL_TEXTURE_2D, NULL);
	}
	glEnable(GL_DEPTH_TEST);
}

void Renderer::renderWorldHud()
{
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	// Get it ONCE instead of every iteration....
	auto* shader = ShaderMap::getInstance()->getShader(WHUD);
	shader->use();
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	for (auto& item : m_worldHudMap) {

		auto& vec = item.second;

		if (vec.size() == 0)
			continue;

		auto* hudObjectDummy = vec[0];

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hudObjectDummy->getTextureID());

		for (size_t i = 0; i < vec.size(); i++)
		{

			auto* worldHud = vec[i];

			if (worldHud->getAlpha() == 0.0f || worldHud->getShouldRender() == false)
				continue;

			shader->setMat4("viewMatrix", m_camera->getViewMat());
			shader->setMat4("projectionMatrix", m_camera->getProjMat());
			shader->setFloat("alphaValue", worldHud->getAlpha());
			shader->setVec2("clip", glm::vec2(worldHud->getXClip(), worldHud->getYClip()));
			shader->setVec3("center", worldHud->getCenter());
			shader->setVec2("scale", worldHud->getScale());
			glBindVertexArray(worldHud->getVAO());

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			glBindVertexArray(0);
		}

		glBindTexture(GL_TEXTURE_2D, NULL);
	}
	m_worldHudMap.clear();

}

void Renderer::renderAndAnimateNetworkingTexts()
{
	if (Client::getInstance()->isConnectedToSever() && Client::getInstance()->isInitialized()) {

		NetGlobals::SERVER_STATE state = Client::getInstance()->getServerState().currentState;

		if (Client::getInstance()->isSpectating() == false) {

			if (Client::getInstance()->getMyData().health == 0) {
				std::string timeText = "Respawn in " + std::to_string(Client::getInstance()->getRespawnTime().timeLeft / 1000) + " seconds";
				unsigned int width = m_text->getTotalWidth(timeText, glm::vec3(0.8f));
				m_text->RenderText(timeText, (SCREEN_WIDTH / 2) - width * 0.5f, 480.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
			}

		}
		else {

			if (m_camera == nullptr) return;

			glm::vec3 modeTextScale = glm::vec3(0.35f);

			std::string modeText = "Mode  ";
			float modeTextWidth = m_text->getTotalWidth(modeText, modeTextScale);

			std::string cameraModeText = "";

			if (m_camera->getSpectatorMode() == SpectatorMode::FreeCamera) {
				cameraModeText = "Free camera";
			}
			else if (m_camera->getSpectatorMode() == SpectatorMode::ThirdPerson) {
				cameraModeText = "Third person";
			}

			float cameraModeWidth = m_text->getTotalWidth(cameraModeText, modeTextScale);
			float totalModeTextWidth = modeTextWidth + cameraModeWidth;
			m_text->RenderText(modeText, (SCREEN_WIDTH / 2) - totalModeTextWidth * 0.5f, (SCREEN_HEIGHT * 0.075f), modeTextScale.x, glm::vec3(1.0f, 1.0f, 1.0f));
			m_text->RenderText(cameraModeText, (SCREEN_WIDTH / 2) - (totalModeTextWidth * 0.5f) + modeTextWidth, (SCREEN_HEIGHT * 0.075f), modeTextScale.x, glm::vec3(1.0f, 0.5f, 0.0f));



			if (m_camera->getSpectatorMode() == SpectatorMode::ThirdPerson) {
				const PlayerPacket* spectatedPlayer = Client::getInstance()->getSpectatedPlayer();

				if (spectatedPlayer == nullptr)
					return;

				glm::vec3 textScale = glm::vec3(0.45f);

				std::string spectateText = "Spectating  ";
				float spectateTextWidth = m_text->getTotalWidth(spectateText, textScale);

				std::string playerName = std::string(spectatedPlayer->userName);
				float playerNameWidth = m_text->getTotalWidth(playerName, textScale);

				float totalWidth = spectateTextWidth + playerNameWidth;

				m_text->RenderText(spectateText, (SCREEN_WIDTH / 2) - totalWidth * 0.5f, (SCREEN_HEIGHT * 0.15f), textScale.x, glm::vec3(1.0f, 1.0f, 1.0f));
				m_text->RenderText(playerName, (SCREEN_WIDTH / 2) - (totalWidth * 0.5f) + spectateTextWidth, (SCREEN_HEIGHT * 0.15f), textScale.x, glm::vec3(1.0f, 0.5f, 0.0f));

			}

		}

	}


}

void Renderer::renderBigNotifications()
{
	std::lock_guard<std::mutex> lockGuard(NetGlobals::PickupNotificationMutex);
	for (size_t i = 0; i < m_bigNotifications.size(); i++) {

		NotificationText& notification = m_bigNotifications[i];

		float xPos = (float)((SCREEN_WIDTH / 2.0f) - notification.width / 2.0f);
		float yPos = (float)(SCREEN_HEIGHT / 1.25f) - ((60.0f * notification.scale.x) * i);

		m_text->RenderText(notification, glm::vec3(xPos, yPos, 0.0f), glm::vec2(notification.scale), notification.useAlpha);

		float lifeTime = notification.lifeTimeInSeconds;
		if (lifeTime == 0.0f)
			lifeTime = 1.0f;

		notification.alphaColor -= DeltaTime * (1.0f / lifeTime);

		if (notification.alphaColor <= 0.0f) {
			m_bigNotifications.erase(m_bigNotifications.begin() + i);
			i--;
		}
	}
}

void Renderer::renderKillFeed()
{
	std::lock_guard<std::mutex> lockGuard(NetGlobals::UpdateKillFeedMutex);
	for (size_t i = 0; i < m_killFeed.size(); i++) {

		NotificationText& notification = m_killFeed[i];

		float xPos = (float)((SCREEN_WIDTH)-notification.width - 25.0f);
		float yPos = (float)(SCREEN_HEIGHT - ((60.0f * notification.scale.x) * (i + 1)));

		m_text->RenderText(notification, glm::vec3(xPos, yPos, 0.0f), glm::vec2(notification.scale), notification.useAlpha);

		float lifeTime = notification.lifeTimeInSeconds;
		if (lifeTime == 0.0f)
			lifeTime = 1.0f;


		notification.alphaColor -= DeltaTime * (1.0f / lifeTime);

		if (notification.alphaColor <= 0.0f) {
			m_killFeed.erase(m_killFeed.begin() + i);
			i--;
		}
	}

	for (size_t i = 0; i < m_killNotification.size(); i++) {

		NotificationText& notification = m_killNotification[i];

		float xPos = (float)((SCREEN_WIDTH / 2) - (notification.width / 2.0f));
		float yPos = (float)((SCREEN_HEIGHT / 2) - ((100.0f * notification.scale.x) * (i + 1)));

		m_text->RenderText(notification, glm::vec3(xPos, yPos, 0.0f), glm::vec2(notification.scale), notification.useAlpha);

		float lifeTime = notification.lifeTimeInSeconds;
		if (lifeTime == 0.0f)
			lifeTime = 1.0f;


		notification.alphaColor -= DeltaTime * (1.0f / lifeTime);

		if (notification.alphaColor <= 0.0f) {
			m_killNotification.erase(m_killNotification.begin() + i);
			i--;
		}
	}

}

void Renderer::createDepthMap() {

	//Create a depth map texture for the rendering system
	glGenFramebuffers(1, &m_depthFBO);

	glGenTextures(1, &m_depthMap);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1, 1, 1, 1 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Renderer::initShaders() {
	ShaderMap* shaderMap = ShaderMap::getInstance();
	Shader* shader;
	shaderMap->createShader(DEPTH_MAP, "Depth.vert", "Depth.frag");
	//Set the light index binding
#if FORWARDPLUS
	shaderMap->createShader(LIGHT_CULL, "LightCullCompute.comp");
	shaderMap->useByName(LIGHT_CULL);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightIndexSSBO);
#endif

#if SSAO
	//Init the compute shader
	shader = shaderMap->createShader(SSAO_RAW, "SSAO.comp");
	shader->use();
	shader->setInt("depthMap", 0);
	shader->setInt("noiseMap", 1);
#endif
#if BLUR 
	shaderMap->createShader(V_BLUR, "Blur/VerticalBlur.comp");		
	shaderMap->createShader(H_BLUR, "Blur/HorizontalBlur.comp");
#endif
#if N_BLUR
	shaderMap->createShader(NAIVE_BLUR, "Blur/Blur.comp");
#endif
	//Set the light index binding
	shaderMap->createShader(BASIC_FORWARD, "VertexShader.vert", "FragShader.frag");
	shaderMap->createShader(ANIMATION, "Animation.vert", "FragShader.frag");
	shaderMap->createShader(SKYBOX, "Skybox.vs", "Skybox.fs");
	shaderMap->getShader(SKYBOX)->setInt("skyBox", 4);
	shaderMap->createShader(FRESNEL, "FresnelFX.vert", "FresnelFX.frag");
	shaderMap->createShader(ENEMYSHIELD, "FresnelFX.vert", "EnemyShield.frag");
	shaderMap->createShader(WHUD, "WorldHud.vs", "WorldHud.fs");
	shaderMap->createShader(TRANSPARENT, "TransparentRender.vert", "TransparentRender.frag");
	m_text = new FreeType();
	m_text->BindTexture();

	/* Hud */
	shader = ShaderMap::getInstance()->createShader(HUD, "HUD.vs", "HUD.fs");
	shader->use();
	shader->setInt("textureSampler", 0);

	shaderMap->createShader(PARTICLES, "Particles.vs", "Particles.gs", "Particles.fs");

	initializeParticle();
}

void Renderer::bindMatrixes(const std::string& shaderName) {
	ShaderMap::getInstance()->getShader(shaderName)->setMat4("viewMatrix", m_camera->getViewMat());
	ShaderMap::getInstance()->getShader(shaderName)->setMat4("projMatrix", m_camera->getProjMat());
}

void Renderer::bindMatrixes(Shader* shader)
{
	shader->setMat4("viewMatrix", m_camera->getViewMat());
	shader->setMat4("projMatrix", m_camera->getProjMat());
}

Renderer* Renderer::getInstance()
{
	if (m_rendererInstance == 0) {
		m_rendererInstance = new Renderer();
	}
	return m_rendererInstance;
}

void Renderer::init(GLFWwindow* window)
{
	m_gWindow = window;
}

void Renderer::setupCamera(Camera* camera)
{
	if (camera == nullptr) {
		return;
	}
	m_camera = camera;
}

void Renderer::submit(GameObject* gameObject, RENDER_TYPE objType)
{
	TextureInfo rings;
	rings.name = "Assets/Textures/Spell_1.png";

	TextureInfo smoke;
	smoke.name = "Assets/Textures/Spell_2.png";

	if (objType == RENDER_TYPE::STATIC)
	{
		//gameObject->addParticle(deathBuffer);
		m_staticObjects.emplace_back(gameObject);
		//m_staticObjects[0]->addParticle(deathBuffer);
		//gameObject->addParticle(deathBuffer);
	}
	else if (objType == RENDER_TYPE::SPELL)
	{
		/* Place the light in the lights list */
		PLIGHT light;
		light.position = gameObject->getObjectTransform().position;
		light.color = glm::vec3(1.0f);
		light.index = m_spells.size();
		m_spells.emplace_back(gameObject);

		Spell* spell = dynamic_cast<Spell*>(gameObject);
		if (spell == nullptr) return;

		if (spell->getType() == OBJECT_TYPE::NORMALATTACK)
		{
			light.attenAndRadius = m_spellHandler->getSpellBase(NORMALATTACK)->m_attenAndRadius;
			light.color = m_spellHandler->getSpellBase(NORMALATTACK)->m_material->diffuse;
			light.strength = m_spellHandler->getSpellBase(NORMALATTACK)->m_strength;
		}

		else if (spell->getType() == OBJECT_TYPE::ENHANCEATTACK)
		{
			light.attenAndRadius = m_spellHandler->getSpellBase(ENHANCEATTACK)->m_attenAndRadius;
			light.color = m_spellHandler->getSpellBase(ENHANCEATTACK)->m_material->diffuse;
			light.strength = m_spellHandler->getSpellBase(ENHANCEATTACK)->m_strength;
		}

		else if (spell->getType() == OBJECT_TYPE::FIRE)
		{
			light.position.y += 2.0f;
			light.attenAndRadius = m_spellHandler->getSpellBase(FIRE)->m_attenAndRadius;
			light.color = m_spellHandler->getSpellBase(FIRE)->m_material->diffuse;
			light.strength = m_spellHandler->getSpellBase(FIRE)->m_strength;
		}

		else if (spell->getType() == OBJECT_TYPE::FLAMESTRIKE)
		{
			light.attenAndRadius = m_spellHandler->getSpellBase(FLAMESTRIKE)->m_attenAndRadius;
			light.color = m_spellHandler->getSpellBase(FLAMESTRIKE)->m_material->diffuse;
			light.strength = m_spellHandler->getSpellBase(FLAMESTRIKE)->m_strength;
		}

		m_lights.emplace_back(light);
	}
	else if (objType == RENDER_TYPE::DYNAMIC) {
		m_dynamicObjects.emplace_back(gameObject);
	}
	else if (objType == RENDER_TYPE::ANIMATEDSTATIC) {
		m_anistaticObjects.emplace_back(gameObject);
	}
	else if (objType == RENDER_TYPE::ANIMATEDDYNAMIC) {
		m_anidynamicObjects.emplace_back(gameObject);
	}
	else if (objType == RENDER_TYPE::PICKUP) {
		m_pickups.emplace_back(gameObject);
	}
	else if (objType == RENDER_TYPE::SHIELD) {
		m_shieldObject = gameObject;
	}
	else if (objType == RENDER_TYPE::ENEMY_SHIELD) {
		m_enemyShieldObject.emplace_back(gameObject);
	}
	else if (objType == RENDER_TYPE::SKYOBJECTS) {
		m_skyObjects.emplace_back(gameObject);
	}
	else if (objType == RENDER_TYPE::POINTLIGHT_SOURCE) {
		/* Place the light in the lights list */
		Pointlight* lightRef = static_cast<Pointlight*>(gameObject);

		PLIGHT light;
		light.position = gameObject->getObjectTransform().position;
		light.color = lightRef->getColor();
		light.attenAndRadius = lightRef->getAttenuationAndRadius(); //First 3 dims are for the attenuation, final 4th is for radius
		light.strength = lightRef->getStrength();
		light.index = -2;

		m_lights.emplace_back(light);
	}
}

void Renderer::submit2DHUD(HudObject* hud)
{
	auto item = m_2DHudMap.find(hud->getTextureID());

	if (item != m_2DHudMap.end()) {
		auto& vec = item._Ptr->_Myval.second;

		vec.emplace_back(hud);
		logTrace("Placed a hud object in an existing vector");
		return;
	}
	else
	{
		std::vector<HudObject*> newVec;
		newVec.reserve(5);
		newVec.emplace_back(hud);
		m_2DHudMap[hud->getTextureID()] = newVec;
		logTrace("Made a new vector for an hud object");
	}
}

void Renderer::submitWorldHud(WorldHudObject* wHud)
{
	auto item = m_worldHudMap.find(wHud->getTextureID());

	if (item != m_worldHudMap.end()) {
		auto& vec = item._Ptr->_Myval.second;

		vec.emplace_back(wHud);
		return;
	}
	else
	{
		std::vector<WorldHudObject*> newVec;
		newVec.reserve(5);
		newVec.emplace_back(wHud);
		m_worldHudMap[wHud->getTextureID()] = newVec;

	}
}

void Renderer::submitSkybox(SkyBox* skybox)
{
	m_skyBox = skybox;
}

void Renderer::submitSpellhandler(SpellHandler* spellhandler)
{
	m_spellHandler = spellhandler;
}

void Renderer::clear() {

	m_staticObjects.clear();
	m_dynamicObjects.clear();
	m_anistaticObjects.clear();
	m_anidynamicObjects.clear();
	m_pickups.clear();
	m_killFeed.clear();
	m_bigNotifications.clear();
	m_spells.clear();
	m_lights.clear();
	m_2DHudMap.clear();
	m_worldHudMap.clear();
	m_enemyShieldObject.clear();
	m_skyObjects.clear();
}

void Renderer::removeRenderObject(GameObject* gameObject, RENDER_TYPE objType)
{
	int index = -1;

	if (objType == RENDER_TYPE::DYNAMIC) { //Remove dynamic objet from the dynamic objet vector
		//Find the index of the object
		for (size_t i = 0; i < m_dynamicObjects.size(); i++)
		{
			if (m_dynamicObjects[i] == gameObject) {
				index = i;
				break;
			}
		}
		if (index > -1) {
			m_dynamicObjects.erase(m_dynamicObjects.begin() + index);
		}
	}
	else if (objType == RENDER_TYPE::SPELL) { //remove spells from the spell vector!!
		int lightIndex = -1;
		//Find the index of the object
		for (size_t i = 0; i < m_spells.size(); i++)
		{
			if (m_spells[i] == gameObject) {
				index = i;
				break;
			}
		}
		if (index > -1) {
			for (size_t i = 0; i < m_lights.size(); i++) {
				if (m_lights[i].index == index) {
					lightIndex = i;
					break;
				}
			}
			m_lights.erase(m_lights.begin() + lightIndex);
			m_spells.erase(m_spells.begin() + index);

			int index = 0;
			for (size_t i = 0; i < m_lights.size(); i++) {
				if (m_lights[i].index != -2) { //If we are not looking at a static pointlight
					if (m_spells[index] == nullptr) {
						return;
					}
					m_lights[i].index = index;
					index++;
				}
			}
		}
	}
	else if (objType == RENDER_TYPE::PICKUP) { //remove spells from the spell vector!!
	   //Find the index of the object
		for (size_t i = 0; i < m_pickups.size(); i++)
		{
			if (m_pickups[i] == gameObject) {
				index = i;
				break;
			}
		}
		if (index > -1) {
			m_pickups.erase(m_pickups.begin() + index);
		}
	}
	else if (objType == STATIC) {
		//Find the index of the object
		for (size_t i = 0; i < m_staticObjects.size(); i++)
		{
			if (m_staticObjects[i] == gameObject) {
				index = i;
				break;
			}
		}
		if (index > -1) {
			m_staticObjects.erase(m_staticObjects.begin() + index);
		}
	}
	else if (objType == RENDER_TYPE::ANIMATEDSTATIC) { //remove PICKUP from the spell PICKUP!!
	   //Find the index of the object
		for (size_t i = 0; i < m_anistaticObjects.size(); i++)
		{
			if (m_anistaticObjects[i] == gameObject) {
				index = i;
				break;
			}
		}
		if (index > -1) {
			m_anistaticObjects.erase(m_anistaticObjects.begin() + index);
		}
	}
}


void Renderer::destroy()
{
	delete m_rendererInstance;
	m_rendererInstance = nullptr;
}


void Renderer::renderSkybox()
{
	glDisable(GL_CULL_FACE);

	//glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	auto* shader = ShaderMap::getInstance()->useByName("Skybox_Shader");
	shader->setMat4("modelMatrix", m_skyBox->getModelMatrix());
	shader->setMat4("viewMatrix", glm::mat4(glm::mat3(m_camera->getViewMat())));
	shader->setMat4("projMatrix", m_camera->getProjMat());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyBox->getCubeMapTexture());
	glBindVertexArray(m_skyBox->getVAO());
	glDrawArrays(GL_TRIANGLES, 0, 36); //Maybe index the skybox?
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, NULL);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}

void Renderer::renderDepthmap() {
	Shader* shader = ShaderMap::getInstance()->useByName(DEPTH_MAP);
	glm::mat4 modelMatrix;
	Mesh* mesh = nullptr;
	//Bind and draw the objects to the depth-buffer
	bindMatrixes(shader);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	//Loop through all of the static Gameobjects
	for (GameObject* object : m_staticObjects)
	{
		if (object == nullptr) {
			continue;
		}

		if (!object->getShouldRender()) {
			continue;
		}

		//Then through all of the meshes
		for (int j = 0; j < object->getMeshesCount(); j++)
		{
			glEnableVertexAttribArray(0);
			modelMatrix = glm::mat4(1.0f);
			//Fetch the current mesh and its transform
			mesh = object->getMesh(j);

			modelMatrix = object->getMatrix(j);

			glBindVertexArray(mesh->getBuffers().vao);

			//Bind the modelmatrix
			shader->setMat4("modelMatrix", modelMatrix);

			glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

			glBindVertexArray(0);
			glDisableVertexAttribArray(0);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_renderedDepthmap = true;
}

void Renderer::render() {
	Mesh* mesh = nullptr;
	Transform transform;
	glm::mat4 modelMatrix;
	Shader* shader = nullptr;
	MeshMap* meshMap = MeshMap::getInstance();
	ShaderMap* shaderMap = ShaderMap::getInstance();
	Material* material = nullptr;
	//We always assume that we haven't rendered a depthmap
	m_renderedDepthmap = false; //This is set to true in renderDepthmap()


#if FORWARDPLUS	
	if (m_lights.size() > 0) {
		renderDepthmap();
	}
#endif

#if SSAO
	if (!m_renderedDepthmap) {
		renderDepthmap();
	}
#endif

#if FORWARDPLUS
	//Light Culling from the compute shader
	if (m_lights.size() > 0) {
		shader = shaderMap->useByName(LIGHT_CULL);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightIndexSSBO);
		bindMatrixes(shader);

		glm::vec2 screenSize = glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
		shader->setVec2("screenSize", screenSize);
		shader->setInt("lightCount", m_lights.size());//Set the number of active pointlights in the scene

		//Bind the depthmap
		glActiveTexture(GL_TEXTURE0);
		shader->setInt("depthMap", 0);
		glBindTexture(GL_TEXTURE_2D, m_depthMap);

		//Send all of the light data into the compute shader
		for (size_t i = 0; i < m_lights.size(); i++) {
			shader->setVec3("lights[" + std::to_string(i) + "].position", m_lights[i].position);
			shader->setFloat("lights[" + std::to_string(i) + "].radius", m_lights[i].attenAndRadius.w);
		}

		glDispatchCompute(workGroups.x, workGroups.y, 1);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		//Unbind the depth & noise
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
#endif

#if SSAO
	//Here we dispatch the SSAO compute shader
	shader = shaderMap->useByName(SSAO_RAW);
	glActiveTexture(GL_TEXTURE0);
	//bindMatrixes(shader);	//Bind view and projection matrix
	glBindTexture(GL_TEXTURE_2D, m_depthMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_SSAONoiseTexture);

	for (size_t i = 0; i < m_SSAOKernels.size(); i++)
	{
		//Assign the kernels
		shader->setVec3("kernels[" + std::to_string(i) + "]", m_SSAOKernels[i]);
	}

	glBindImageTexture(0, m_SSAOColourBuffer, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F); //Bind the image unit

	glDispatchCompute(workGroups.x, workGroups.y, 1);
	glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
	glBindTexture(GL_TEXTURE_2D, 0);
#if N_BLUR //Naive blurring
	shader = shaderMap->useByName(NAIVE_BLUR);
	glBindImageTexture(0, m_SSAOColourBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glDispatchCompute(workGroups.x, workGroups.y, 1);
	glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
#endif
#if BLUR //More efficiant blurring
	//Blur Horizontally
	shader = shaderMap->useByName(H_BLUR);
	glBindImageTexture(0, m_SSAOColourBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F); //Bind the image unit
	glDispatchCompute(2, 1, 1); //Dispatch 2 - xAxis workgroups
	glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Blur Vertically
	shader = shaderMap->useByName(V_BLUR);
	glBindImageTexture(0, m_SSAOColourBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F); //Bind the image unit
	glDispatchCompute(1, 1, 1); //Because of our resolution we can comfortably dipatch one group of threads
	glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
	glBindTexture(GL_TEXTURE_2D, 0);
#endif

#endif

#pragma region Color_Render
	shader = shaderMap->useByName(BASIC_FORWARD);
	shader->clearBinding();

	if (Client::getInstance()->getMyData().health <= 0) {
		shader->setInt("grayscale", 1);
	}
	else {
		shader->setInt("grayscale", 0);
	}

	//Bind view- and projection matrix
	bindMatrixes(shader);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightIndexSSBO);
	shader->setVec3("CameraPosition", m_camera->getCamPos());
	//Add a step where we insert lights into the scene

	shader->setInt("LightCount", m_lights.size());
	shader->setInt("SSAO", 0);

	if (m_lights.size() > 0) {
		std::string iConv = "";
		for (size_t i = 0; i < m_lights.size(); i++) {
			iConv = std::to_string(i);

			if (m_lights[i].index != -2) {
				shader->setVec3("pLights[" + std::to_string(i) + "].position", m_spells[m_lights[i].index]->getObjectTransform().position);
			}
			else {
				shader->setVec3("pLights[" + std::to_string(i) + "].position", m_lights[i].position);
			}
			shader->setVec3("pLights[" + iConv + "].color", m_lights[i].color);
			shader->setVec4("pLights[" + iConv + "].attenAndRadius", m_lights[i].attenAndRadius);
			shader->setFloat("pLights[" + iConv + "].strength", m_lights[i].strength);
		}
	}

#if SSAO 
	shader->setInt("SSAO", 1);
	glBindImageTexture(0, m_SSAOColourBuffer, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
#endif

	//Render Static objects
	for (GameObject* object : m_staticObjects)
	{
		if (object == nullptr) {
			continue;
		}

		if (!object->getShouldRender()) {
			continue;
		}
		//TODO
		//object->UpdateParticles(dt);
		//object->UpdateParticles(dt);
		//object->RenderParticles(glm::vec3(0), m_camera);

		//Then through all of the meshes
		for (int j = 0; j < object->getMeshesCount(); j++)
		{
			//Fetch the current mesh and its transform
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			mesh = object->getMesh(j);			

			//Bind the material
			object->bindMaterialToShader(shader, j);

			//Bind the modelmatrix
			modelMatrix = object->getMatrix(j);
			shader->setMat4("modelMatrix", modelMatrix);

			glBindVertexArray(mesh->getBuffers().vao);
			glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

			glBindVertexArray(0);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);			
		}
		object->RenderParticles(m_camera);
		//object->getTransform().position
	}
	shader->setInt("SSAO", 0);
	shader->clearBinding();
	//Dynamic objects
	if (m_dynamicObjects.size() > 0) {
		for (GameObject* object : m_dynamicObjects)
		{
			if (object == nullptr) {
				continue;
			}

			if (!object->getShouldRender()) {
				continue;
			}

			//Then through all of the meshes
			for (int j = 0; j < object->getMeshesCount(); j++)
			{
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);
				glEnableVertexAttribArray(3);
				mesh = object->getMesh(j);
				//Bind the material
				object->bindMaterialToShader(shader, j);

				//Bind the modelmatrix
				modelMatrix = object->getMatrix(j);
				shader->setMat4("modelMatrix", modelMatrix);

				glBindVertexArray(mesh->getBuffers().vao);
				glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

				glBindVertexArray(0);
				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);
				glDisableVertexAttribArray(3);
			}
		}
	}
	shader->clearBinding();

	//Pickup objects
	if (m_pickups.size() > 0) {
		for (GameObject* object : m_pickups)
		{
			if (object == nullptr) {
				continue;
			}

			if (!object->getShouldRender()) {
				continue;
			}

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);

			Pickup* p = dynamic_cast<Pickup*>(object);

			//Fetch the current mesh and its transform
			mesh = p->getRenderInformation().mesh;
			glBindVertexArray(mesh->getBuffers().vao);

			//Bind the material
			shader->setMaterial(p->getRenderInformation().material);

			//Bind the modelmatrix
			glm::mat4 mMatrix = glm::mat4(1.0f);
			mMatrix = glm::translate(mMatrix, p->getObjectTransform().position);
			mMatrix *= glm::mat4_cast(p->getObjectTransform().rotation);
			mMatrix = glm::scale(mMatrix, p->getObjectTransform().scale);

			shader->setMat4("modelMatrix", mMatrix);

			glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

			glBindVertexArray(0);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);
		}
	}
	shader->clearBinding();

	if (m_skyObjects.size() > 0)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		shader = shaderMap->useByName(TRANSPARENT);
		bindMatrixes(shader);
		//Render objects 
		float rotValue = 0.0f;

		for (GameObject* object : m_skyObjects)
		{
			if (object == nullptr)
				continue;
			if (!object->getShouldRender())
				continue;

			rotValue -= 0.3f;

			for (int j = 0; j < object->getMeshesCount(); j++)
			{
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);
				mesh = object->getMesh(j);

				material = object->getMaterial(j);
				object->bindMaterialToShader(shader, j);

				modelMatrix = glm::mat4(1.0f); //<--- Change this line to apply rotation
				modelMatrix = object->getMatrix(j);


				modelMatrix = glm::rotate(modelMatrix, (float)glfwGetTime() * rotValue, glm::vec3(0.0f, 1.0f, 0.0f));
				shader->setMat4("modelMatrix", modelMatrix);

				glBindVertexArray(mesh->getBuffers().vao);
				glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

				glBindVertexArray(0);
				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);
			}
		}
		shader->clearBinding();
	}

#pragma endregion


#pragma region Animation_Render
	//TODO: Evaluate this implementation, should be an easier way to bind values to shaders as they're changed
	// Possibly extract functions. Only difference in rendering is the shader and the binding of bone matrices
	if (m_anistaticObjects.size() > 0) {
		shader = shaderMap->useByName(ANIMATION);

		//Bind view- and projection matrix
		bindMatrixes(shader);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightIndexSSBO);

		//Add a step where we insert lights into the scene
		shader->setInt("LightCount", m_spells.size());
		if (m_lights.size() > 0) {
			std::string iConv = "";
			for (size_t i = 0; i < m_lights.size(); i++) {
				iConv = std::to_string(i);

				if (m_lights[i].index != -2) {
					shader->setVec3("pLights[" + std::to_string(i) + "].position", m_spells[m_lights[i].index]->getObjectTransform().position);
				}
				else {
					shader->setVec3("pLights[" + std::to_string(i) + "].position", m_lights[i].position);
				}

				shader->setVec3("pLights[" + iConv + "].color", m_lights[i].color);

				shader->setVec4("pLights[" + iConv + "].attenAndRadius", m_lights[i].attenAndRadius);
			}
		}
		for (GameObject* object : m_anistaticObjects)
		{
			if (object == nullptr) {
				continue;
			}

			if (!object->getShouldRender()) {
				continue;
			}

			//Then through all of the meshes
			AnimatedObject* animObj = dynamic_cast<AnimatedObject*>(object);

			if (animObj == nullptr) continue;

			for (int j = 0; j < object->getMeshesCount(); j++)
			{
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);
				//Fetch the current mesh and its transform
				mesh = object->getMesh(j);
				transform = object->getTransform(j);

				//Bind calculated bone matrices
				animObj->BindAnimation(j);

				//Bind the material
				object->bindMaterialToShader(ANIMATION, j);

				modelMatrix = glm::mat4(1.0f);
				modelMatrix = object->getMatrix(j);

				//Bind the modelmatrix
				shader->setMat4("modelMatrix", modelMatrix);

				glBindVertexArray(mesh->getBuffers().vao);

				glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);
				glBindVertexArray(0);
				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);
			}
		}
		shader->clearBinding();
	}


#pragma endregion
	renderSkybox();
	// Spell Rendering
	m_spellHandler->renderSpell();

#ifdef DEBUG_WIREFRAME
	// DEBUG (MOSTLY FOR DSTR)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
	//Enemy Deflect Render
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	shader = shaderMap->useByName(ENEMYSHIELD);

	//Bind view- and projection matrix
	bindMatrixes(shader);

	shader->setVec3("CameraPosition", m_camera->getCamPos());
	//Add a step where we insert lights into the scene
	shader->setInt("LightCount", m_spells.size());

	//Render Deflect Objects
	for (GameObject* object : m_enemyShieldObject)
	{
		//Then through all of the meshes
		for (int j = 0; j < object->getMeshesCount(); j++)
		{
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			//Fetch the current mesh and its transform
			mesh = object->getMesh(j);
			shader->setFloat("time", glfwGetTime());
			//Bind the material
			object->bindMaterialToShader(shader, j);

			modelMatrix = glm::mat4(1.0f);

			modelMatrix = object->getMatrix(j);
			//Bind the modelmatrix
			shader->setMat4("modelMatrix", modelMatrix);

			glBindVertexArray(mesh->getBuffers().vao);

			glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

			glBindVertexArray(0);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}
	}

	for (size_t i = 0; i < m_enemyShieldObject.size(); i++)
	{
		delete m_enemyShieldObject[i];
	}

	m_enemyShieldObject.clear();
	shader->clearBinding();
	
	//Deflect Render
	if (m_shieldObject != nullptr && m_shieldObject->getShouldRender())
	{
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LEQUAL);
		shader = shaderMap->useByName(FRESNEL);
		bindMatrixes(shader);

		mesh = m_shieldObject->getMesh();
		m_shieldObject->bindMaterialToShader(shader);

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = m_shieldObject->getMatrix();
		
		shader->setVec3("CameraPosition", m_camera->getCamPos());
		shader->setFloat("time", glfwGetTime());
		shader->setMat4("modelMatrix", modelMatrix);

		glBindVertexArray(mesh->getBuffers().vao);
		glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);
		glBindVertexArray(0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		shader->clearBinding();
	}


	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	TextRenderer::getInstance()->renderText();
	

	renderWorldHud();

	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);

	glDisable(GL_CULL_FACE);

	renderAndAnimateNetworkingTexts();
	renderBigNotifications();
	renderKillFeed();
	renderHUD();
	m_enemyShieldObject.clear();
}


void Renderer::renderSpell(SpellHandler* spellHandler)
{
	Mesh* meshRef;
	Shader* shader = ShaderMap::getInstance()->getShader(BASIC_FORWARD);
	Transform meshTransform;
	bindMatrixes(shader); //We only need to bind this once, seeing as though we are using only one shader
	int psSelector = 0;
	for (size_t i = 0; i < m_spells.size(); i++)
	{
		if (m_spells[i]->getType() == OBJECT_TYPE::POINTLIGHT) {
			continue;
		}

		ShaderMap::getInstance()->useByName(BASIC_FORWARD);
		meshTransform = m_spells[i]->getObjectTransform();

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, meshTransform.position);
		modelMatrix = glm::scale(modelMatrix, meshTransform.scale);
		modelMatrix *= glm::mat4_cast(meshTransform.rotation);

		shader->setMat4("modelMatrix", modelMatrix);

		if (m_spells[i]->getType() == OBJECT_TYPE::NORMALATTACK)
		{
			meshRef = spellHandler->getSpellBase(NORMALATTACK)->m_mesh;
			glBindVertexArray(meshRef->getBuffers().vao);
			shader->setMaterial(spellHandler->getSpellBase(NORMALATTACK)->m_material);
			glDrawElements(GL_TRIANGLES, meshRef->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

			glBindVertexArray(0);
		}
		else if (m_spells[i]->getType() == OBJECT_TYPE::ENHANCEATTACK)
		{
			meshRef = spellHandler->getSpellBase(ENHANCEATTACK)->m_mesh;
			glBindVertexArray(meshRef->getBuffers().vao);
			shader->setMaterial(spellHandler->getSpellBase(ENHANCEATTACK)->m_material);
			glDrawElements(GL_TRIANGLES, meshRef->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

			glBindVertexArray(0);
		}
		else if (m_spells[i]->getType() == OBJECT_TYPE::FLAMESTRIKE)
		{
			meshRef = spellHandler->getSpellBase(FLAMESTRIKE)->m_mesh;
			glBindVertexArray(meshRef->getBuffers().vao);
			shader->setMaterial(spellHandler->getSpellBase(FLAMESTRIKE)->m_material);
			glDrawElements(GL_TRIANGLES, meshRef->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);
			glBindVertexArray(0);
		}

		else if (m_spells[i]->getType() == OBJECT_TYPE::FIRE)
		{
			meshRef = spellHandler->getSpellBase(FIRE)->m_mesh;
			glBindVertexArray(meshRef->getBuffers().vao);
			shader->setMaterial(spellHandler->getSpellBase(FIRE)->m_material);
			//glDrawElements(GL_TRIANGLES, meshRef->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);
			glBindVertexArray(0);
		}


		for (int j = 0; j < m_spells[i]->getParticles().size(); j++)
		{
			m_spells[i]->getParticles()[j].Render(m_camera);
			m_spells[i]->getParticles()[j].SetPosition(meshTransform.position);
		}
	}

	for (int i = 0; i < m_particleSystems.size(); i++)
	{
		m_particleSystems[i].Render(m_camera);
		m_particleSystems[i].SetPosition(glm::vec3(0));
	}
}

void Renderer::addBigNotification(NotificationText notification)
{
	m_bigNotifications.push_back(notification);
}

void Renderer::addKillFeed(NotificationText notification)
{
	m_killFeed.push_back(notification);
}

void Renderer::addKillNotification(NotificationText notification)
{
	m_killNotification.push_back(notification);
}

unsigned int Renderer::getTextWidth(const std::string& text, const glm::vec3& scale)
{
	return m_text->getTotalWidth(text, scale);
}

Camera* Renderer::getMainCamera() const
{
	return m_camera;
}

void Renderer::initializeParticle()
{
	PSinfo tempPS;
	TextureInfo tempTxt;

	tempTxt.name = "Assets/Textures/betterSmoke.png";
	tempPS.width = 0.9f;
	tempPS.heigth = 1.2f;
	tempPS.lifetime = 5.0f;
	tempPS.maxParticles = 300;
	tempPS.emission = 0.02f;
	tempPS.force = -0.54f;
	tempPS.drag = 0.0f;
	tempPS.gravity = -2.2f;
	tempPS.seed = 1;
	tempPS.cont = true;
	tempPS.omnious = true;
	tempPS.spread = 5.0f;
	tempPS.glow = 1.3;
	tempPS.scaleDirection = 0;
	tempPS.swirl = 0;
	tempPS.fade = 1;
	tempPS.randomSpawn = true;
	tempPS.color = glm::vec3(0.3f, 0.3f, 0.3f);
	tempPS.blendColor = glm::vec3(1.0f, 1.0f, 1.0f);
	tempPS.color = glm::vec3(0.0, 0.0f, 0.0f);
	tempPS.direction = glm::vec3(0.0f, -1.0f, 0.0f);

	deathBuffer = new ParticleBuffers(tempPS, tempTxt);
	deathBuffer->setShader(ShaderMap::getInstance()->getShader(PARTICLES)->getShaderID());
	deathBuffer->bindBuffers();

	//m_staticObjects.back()->addParticle(deathBuffer);
}

void Renderer::updateParticles(float dt)
{
	for (GameObject* object : m_staticObjects)
	{
		object->UpdateParticles(dt);
	}

	for (int i = 0; i < m_particleSystems.size(); i++)
	{
		m_particleSystems[i].Update(dt);
	}
}

void Renderer::removePoof()
{
	//m_particleSystems.erase(m_particleSystems[0]);
}

void Renderer::death()
{
	//m_particleSystems.emplace_back(deathBuffer);
}