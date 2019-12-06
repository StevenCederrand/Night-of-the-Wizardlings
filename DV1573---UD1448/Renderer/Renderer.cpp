#include <Pch/Pch.h>
#include "Renderer.h"
#include <Networking/Client.h>
#include "TextRenderer.h"

#define TILE_SIZE 16

Renderer* Renderer::m_rendererInstance = 0;

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

}

Renderer::~Renderer()
{
	//delete m_bloom;
	delete m_text;
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

		if (state == NetGlobals::SERVER_STATE::GameInSession) {

		/*	uint32_t minutes = Client::getInstance()->getRoundTimePacket().minutes;
			uint32_t seconds = Client::getInstance()->getRoundTimePacket().seconds;
			std::string timeText = std::to_string(minutes) + ":";

			if (seconds >= 10) {
				timeText += std::to_string(seconds);
			}

			else {
				timeText += "0" + std::to_string(seconds);
			}

			timeText = "Time Left: " + timeText;
			glm::vec3 scale = glm::vec3(0.5f, 0.5f, 0.5f);
			float width = m_text->getTotalWidth(timeText, scale);

			m_text->RenderText(timeText, (SCREEN_WIDTH / 2) - width * 0.5f, (SCREEN_HEIGHT * 0.95f), scale.x, glm::vec3(1.0f, 1.0f, 1.0f));*/
		}
		else if (state == NetGlobals::SERVER_STATE::WaitingForPlayers) {

			int numberOfPlayersReady = Client::getInstance()->getNumberOfReadyPlayers();
			int numberOfPlayers = Client::getInstance()->getNumberOfPlayers();
			glm::vec3 scale = glm::vec3(0.55f);
			glm::vec3 baseColor = glm::vec3(1.0f);
			std::string numberOfReadyPlayersText = std::to_string(numberOfPlayersReady) + "/" + std::to_string(numberOfPlayers) + " players ready";
			if (!Client::getInstance()->isSpectating()) {
				bool meReady = Client::getInstance()->getMyData().isReady;

				std::string readyBase = "You are ";
				std::string readyText = meReady ? "Ready" : "Not Ready";
				glm::vec3 readyColor = meReady ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);


				unsigned int readyBaseWidth = m_text->getTotalWidth(readyBase, scale);
				unsigned int readyWidth = m_text->getTotalWidth(readyText, scale);
				unsigned int totalWidth = readyBaseWidth + readyWidth;

				m_text->RenderText(readyBase, (SCREEN_WIDTH / 2) - totalWidth * 0.5f, SCREEN_HEIGHT * 0.35f, scale.x, baseColor);
				m_text->RenderText(readyText, (SCREEN_WIDTH / 2) - totalWidth * 0.5f + readyBaseWidth, SCREEN_HEIGHT * 0.35f, scale.x, readyColor);

				if (meReady == false) {
					glm::vec3 howtoScale = glm::vec3(0.35f);
					std::string howToReadyText = "Press F1 to ready";
					unsigned int width = m_text->getTotalWidth(howToReadyText, howtoScale);

					m_text->RenderText(howToReadyText, (SCREEN_WIDTH / 2) - width * 0.5f, SCREEN_HEIGHT * 0.30f, howtoScale.x, baseColor);
				}

			}

			unsigned int playersThatAreReadyWidth = m_text->getTotalWidth(numberOfReadyPlayersText, glm::vec3(0.40f));
			m_text->RenderText(numberOfReadyPlayersText, (SCREEN_WIDTH / 2) - playersThatAreReadyWidth * 0.5f, SCREEN_HEIGHT * 0.92f, 0.40f, baseColor);

		}
		else if (state == NetGlobals::SERVER_STATE::GameFinished) {
			/*uint32_t minutes = Client::getInstance()->getRoundTimePacket().minutes;
			uint32_t seconds = Client::getInstance()->getRoundTimePacket().seconds;
			std::string timeText = std::to_string(minutes) + ":";

			if (seconds >= 10) {
				timeText += std::to_string(seconds);
			}
			else {
				timeText += "0" + std::to_string(seconds);
			}

			timeText = "End of round: " + timeText;
			unsigned int width = m_text->getTotalWidth(timeText, glm::vec3(0.5f));
			m_text->RenderText(timeText, SCREEN_WIDTH / 2 - 135.0f, 680.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));*/
		}

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

		float xPos = (float)((SCREEN_WIDTH) - notification.width - 25.0f);
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
		float yPos = (float)((SCREEN_HEIGHT / 2) - ((90.0f * notification.scale.x) * (i + 1)));

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

	//Inits the shaders for forward+
	initShaders();
}

void Renderer::initShaders() {
	ShaderMap::getInstance()->createShader(DEPTH_MAP, "Depth.vert", "Depth.frag");
	//Set the light index binding
	ShaderMap::getInstance()->createShader(LIGHT_CULL, "LightCullCompute.comp");
	ShaderMap::getInstance()->useByName(LIGHT_CULL);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightIndexSSBO);
	ShaderMap::getInstance()->createShader(BASIC_FORWARD, "VertexShader.vert", "FragShader.frag");
	ShaderMap::getInstance()->createShader(ANIMATION, "Animation.vert", "FragShader.frag");
	ShaderMap::getInstance()->createShader("Skybox_Shader", "Skybox.vs", "Skybox.fs");
	ShaderMap::getInstance()->getShader("Skybox_Shader")->setInt("skyBox", 4);	
	ShaderMap::getInstance()->createShader(FRESNEL, "FresnelFX.vert", "FresnelFX.frag");
	ShaderMap::getInstance()->createShader(ENEMYSHIELD, "FresnelFX.vert", "EnemyShield.frag");
	ShaderMap::getInstance()->createShader(TRANSPARENT, "TransparentRender.vert", "TransparentRender.frag");
	ShaderMap::getInstance()->createShader(WHUD, "WorldHud.vs", "WorldHud.fs");

	/*=====================================================*/
	/*ShaderMap::getInstance()->createShader(BLOOM, "Bloom.vs", "Bloom.fs");
	ShaderMap::getInstance()->useByName(BLOOM);
	ShaderMap::getInstance()->getShader(BLOOM)->setInt("albedoTexture", 0);

	ShaderMap::getInstance()->createShader(BLUR, "Blur.vs", "Blur.fs");
	ShaderMap::getInstance()->useByName(BLUR);
	ShaderMap::getInstance()->getShader(BLUR)->setInt("brightImage", 0);

	ShaderMap::getInstance()->createShader(BLOOM_BLUR, "BloomBlur.vs", "BloomBlur.fs");
	ShaderMap::getInstance()->useByName(BLOOM_BLUR);
	ShaderMap::getInstance()->getShader(BLOOM_BLUR)->setInt("sceneImage", 0);
	ShaderMap::getInstance()->getShader(BLOOM_BLUR)->setInt("bloomImage", 1);*/
	m_text = new FreeType();
	m_text->BindTexture();
	//m_bloom = new BloomBlur;
	//m_bloom->createHdrFBO();
	//m_bloom->createPingPingFBO();
	/*=====================================================*/

	/* Hud */
	auto* shader = ShaderMap::getInstance()->createShader(HUD, "HUD.vs", "HUD.fs");
	shader->use();
	shader->setInt("textureSampler", 0);

	ShaderMap::getInstance()->createShader(PARTICLES, "Particles.vs", "Particles.gs", "Particles.fs");

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
		m_staticObjects.emplace_back(gameObject);
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
		}

		else if (spell->getType() == OBJECT_TYPE::ENHANCEATTACK)
		{
			light.attenAndRadius = m_spellHandler->getSpellBase(ENHANCEATTACK)->m_attenAndRadius;
			light.color = m_spellHandler->getSpellBase(ENHANCEATTACK)->m_material->diffuse;
		}

		else if (spell->getType() == OBJECT_TYPE::FIRE)
		{
			light.position.y += 2.0f;
			light.attenAndRadius = m_spellHandler->getSpellBase(FIRE)->m_attenAndRadius;
			light.color = m_spellHandler->getSpellBase(FIRE)->m_material->diffuse;
		}

		else if (spell->getType() == OBJECT_TYPE::FLAMESTRIKE)
		{
			light.attenAndRadius = m_spellHandler->getSpellBase(FLAMESTRIKE)->m_attenAndRadius;
			light.color = m_spellHandler->getSpellBase(FLAMESTRIKE)->m_material->diffuse;
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

void Renderer::render() {
	Mesh* mesh = nullptr;
	Transform transform;
	glm::mat4 modelMatrix;
	Shader* shader = nullptr;
	MeshMap* meshMap = MeshMap::getInstance();
	ShaderMap* shaderMap = ShaderMap::getInstance();
	Material* material = nullptr;
	
	
#pragma region Depth_Render & Light_Cull
	if (m_lights.size() > 0) {
		shader = shaderMap->useByName(DEPTH_MAP);

		//Bind and draw the objects to the depth-buffer
		bindMatrixes(shader);
		glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//Loop through all of the gameobjects
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

		//Animated static objects
		//TODO: Consider animation for the depth shader
		for (GameObject* object : m_anistaticObjects)
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

		for (GameObject* object : m_pickups)
		{
			if (object == nullptr) {
				continue;
			}

			if (!object->getShouldRender()) {
				continue;
			}

			Pickup* p = dynamic_cast<Pickup*>(object);

			//Then through all of the meshes
			for (int j = 0; j < object->getMeshesCount(); j++)
			{
				glEnableVertexAttribArray(0);
				modelMatrix = glm::mat4(1.0f);
				//Fetch the current mesh and its transform

				mesh = p->getRenderInformation().mesh;

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

#pragma region Light_Culling
		shader = shaderMap->useByName(LIGHT_CULL);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightIndexSSBO);
		bindMatrixes(shader);

		glm::vec2 screenSize = glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
		shader->setVec2("screenSize", screenSize);
		shader->setInt("lightCount", m_lights.size());//Set the number of active pointlights in the scene

		//Bind the depthmap
		glActiveTexture(GL_TEXTURE0);
		shader->setInt("depthMap", 0); //Not sure if this has to happen every frame
		glBindTexture(GL_TEXTURE_2D, m_depthMap);

		//Send all of the light data into the compute shader
		for (size_t i = 0; i < m_lights.size(); i++) {
			shader->setVec3("lights[" + std::to_string(i) + "].position", m_lights[i].position);
			shader->setFloat("lights[" + std::to_string(i) + "].radius", m_lights[i].attenAndRadius.w);
		}

		glDispatchCompute(workGroups.x, workGroups.y, 1);

		glMemoryBarrier(GL_SHADER_STORAGE_BUFFER);
		//Unbind the depth
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
#pragma endregion
	}

	
	
	//BLOOMBLUR MISSION STEP 1: SAMPLE
	//m_bloom->bindHdrFBO();

#ifdef DEBUG_WIREFRAME
	// DEBUG (MOSTLY FOR DSTR)
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_M) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_N) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

	//Render Static objects
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
			//Fetch the current mesh and its transform
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
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
		}
	}

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
	}
	shader->clearBinding();
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
	}

	shader->clearBinding();
#pragma endregion
	renderSkybox();
	// Spell Rendering
	m_spellHandler->renderSpell();

#ifdef DEBUG_WIREFRAME
	// DEBUG (MOSTLY FOR DSTR)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

	//ShaderMap::getInstance()->useByName(BLUR);
	//ShaderMap::getInstance()->getShader(BLUR)->setInt("horizontal", m_bloom->getHorizontal() ? 1 : 0);
	//m_bloom->blurIteration(0);
	//for (unsigned int i = 0; i < m_bloom->getAmount() - 1; i++)
	//{
	//	ShaderMap::getInstance()->getShader(BLUR)->setInt("horizontal", m_bloom->getHorizontal() ? 1 : 0);
	//	m_bloom->blurIteration(1);
	//}
	//m_bloom->unbindTextures();
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//ShaderMap::getInstance()->useByName(BLOOM_BLUR);
	//If the client is dead
	//
	//if (Client::getInstance()->getMyData().health <= 0) {
	//	ShaderMap::getInstance()->getShader(BLOOM_BLUR)->setInt("grayscale", 1);
	//}
	//else {
	//	ShaderMap::getInstance()->getShader(BLOOM_BLUR)->setInt("grayscale", 0);
	//}
	//m_bloom->sendTextureLastPass();
	//m_bloom->renderQuad();
	//m_bloom->unbindTextures();

#pragma region Enemy_Deflect_Render
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
		}
	}

	for (size_t i = 0; i < m_enemyShieldObject.size(); i++)
	{
		delete m_enemyShieldObject[i];
	}

	m_enemyShieldObject.clear();
	shader->clearBinding();
#pragma endregion

#pragma region Deflect_Render
	if (m_shieldObject->getShouldRender())
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LEQUAL);
		shader = shaderMap->useByName(FRESNEL);
		bindMatrixes(shader);
	
		mesh = m_shieldObject->getMesh();
		m_shieldObject->bindMaterialToShader(shader);

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = m_shieldObject->getMatrix();
		shader->setInt("LightCount", m_spells.size());
		shader->setVec3("CameraPosition", m_camera->getCamPos());
		shader->setFloat("time", glfwGetTime());
		shader->setMat4("modelMatrix", modelMatrix);

		glBindVertexArray(mesh->getBuffers().vao);
		glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);
		glBindVertexArray(0);

		shader->clearBinding();
	}
#pragma endregion

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

}

void Renderer::updateParticles(float dt)
{

}
