#include <Pch/Pch.h>
#include "PlayState.h"
#include <System/StateManager.h>
#include "MenuState.h"
#include <Networking/Client.h>
#include <Networking/LocalServer.h>
#include <Renderer/TextRenderer.h>
#include <BetterText/TextManager.h>
#define PLAYSECTION "PLAYSTATE"

#define SHOW_MEMORY_INFO true

void logVec3(glm::vec3 vector) {
	logTrace("Vector: ({0}, {1}, {2})", std::to_string(vector.x), std::to_string(vector.y), std::to_string(vector.z));
}

PlayState::PlayState(bool spectator)
{
	Shader* basicTempShader = ShaderMap::getInstance()->getShader(BASIC_FORWARD);
	basicTempShader->use();
	basicTempShader->setInt("albedoTexture", 0);
	basicTempShader->setInt("normalMap", 1);	
	
	m_camera = new Camera();
	mu.printBoth("After physics and camera init:");

	TextRenderer::getInstance()->init(m_camera);
	TextManager::getInstance();
	mu.printBoth("After Font:");

	if (spectator == false) {

		// To get the height of the character at start due to bounding box calculations.
		GameObject* AnimationMesh = new WorldObject("AnimationMesh");
		AnimationMesh->loadMesh("NyCharacter.mesh");
		delete AnimationMesh;

		GameObject* fpsShield = new ShieldObject("PlayerShield");
		fpsShield->loadMesh("ShieldMeshFPS.mesh");
		delete fpsShield;

		GameObject* enemyShield = new EnemyShieldObject("enemyShield");
		enemyShield->loadMesh("EnemyShieldMesh.mesh");
		delete enemyShield;

		m_spellHandler = new SpellHandler();
		m_spellHandler->setOnHitCallback(std::bind(&PlayState::onSpellHit_callback, this));

		m_player = new Player("Player", NetGlobals::PlayerFirstSpawnPoint, m_camera, m_spellHandler);
		m_player->setHealth(NetGlobals::PlayerMaxHealth);

		if (Client::getInstance()->isInitialized())
			Client::getInstance()->assignSpellHandler(m_spellHandler);

		mu.printBoth("After fps shield, enemy shield and animationMesh:");

		m_hudHandler.loadPlayStateHUD();

		mu.printBoth("After hud:");
	}
	else {
		m_spellHandler = new SpellHandler();
		m_spellHandler->setOnHitCallback(std::bind(&PlayState::onSpellHit_callback, this));
		m_camera->setSpectatorMode(SpectatorMode::FreeCamera);
	}

	Renderer* renderer = Renderer::getInstance();
	renderer->setupCamera(m_camera);
	mu.printBoth("After renderer:");

	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();
	mu.printBoth("After Skybox:");
	renderer->submitSkybox(m_skybox);
	renderer->submitSpellhandler(m_spellHandler);

	// Map
	loadMap();

	//Load test cube for normal mapping
	/*GameObject* tangentCube = new WorldObject();
	tangentCube->loadMesh("tangentCube.mesh");
	tangentCube->setTexture("NormalMap/Bricks01.jpg");
	tangentCube->setNormalMap("NormalMap/BricksNRM.jpg");
	m_objects.push_back(tangentCube);	
	renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::STATIC);*/

	// Geneterate bullet objects / hitboxes
	gContactAddedCallback = callbackFunc;

	if(Client::getInstance()->isInitialized())
		Client::getInstance()->assignSpellHandler(m_spellHandler);

	m_hideHUD = false;

	InitParticle();

#if SHOW_MEMORY_INFO
	std::string memTex = "Ram: " + std::to_string(mu.getCurrentRamUsage()) + " | VRam: " + std::to_string(mu.getCurrentVramUsage()) +
		" | Highest Ram: " + std::to_string(mu.getHighestRamUsage()) + " | Highest VRam: " + std::to_string(mu.getHighestVramUsage());
	
	m_memoryText = TextManager::getInstance()->addDynamicText(
		memTex,
		0.09f,
		glm::vec3(0.0f, 0.0f, 0.0f),
		1.f,
		TextManager::TextBehaviour::StayForever,
		glm::vec3(0.0f, 0.0f, 0.0f), true);
#endif
	mu.printBoth("End of play state init:");

	m_fpsText = TextManager::getInstance()->addDynamicText(
		"fps: " + std::to_string(Framerate),
		0.09f,
		glm::vec3(-0.95f, -0.02f, 0.0f),
		1.f,
		TextManager::TextBehaviour::StayForever,
		glm::vec3(0.0f, 0.0f, 0.0f), true);


}

void PlayState::loadMap()
{
	Renderer* renderer = Renderer::getInstance();

	// Map objects
	switch (m_map)
	{
	case 0:
		m_objects.push_back(new MapObject("Academy_Map"));
		m_objects[m_objects.size() - 1]->loadMesh("Towermap/Academy_t.mesh");
		renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::STATIC);
		break;

	case 1:
		m_objects.push_back(new MapObject("Debug_Map"));
		m_objects[m_objects.size() - 1]->loadMesh("Debug_Map.mesh");
		renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::STATIC);
		break;

	default:
		break;
	}
	mu.printBoth("After map objects:");

	// Collision // TODO: Move to object constructor
	for (GameObject* g : m_objects)
		g->makeStatic();
	mu.printBoth("After rigidbodies:");

	loadSkyDebris(renderer);
	mu.printBoth("After sky debris objects:");

	// Non-hitbox decoraction
	loadDecor();
	mu.printBoth("After decor objects:");

	// Lights
	loadLights();
	mu.printBoth("After point lights:");

	// Destuction
	loadDestructables();
	mu.printBoth("After destructables:");

}

void PlayState::loadSkyDebris(Renderer* renderer)
{
	m_objects.push_back(new MapObject("Debris_Far"));
	m_objects[m_objects.size() - 1]->loadMesh("StonePlaneFar.mesh");
	m_objects[m_objects.size() - 1]->setTransform(glm::vec3(0.0f, 800.0f, 0.0f), glm::quat(glm::vec3(0.f, 0.f, 0.f)), glm::vec3(7.f));
	renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::SKYOBJECTS);

	//Create two or three more planes that are handeld as sky objects, then randomize their rotation speed.
	m_objects.push_back(new MapObject("Debris_Mid"));
	m_objects[m_objects.size() - 1]->loadMesh("StonePlaneFar.mesh");
	m_objects[m_objects.size() - 1]->setTransform(glm::vec3(0.0f, 300.0f, 0.0f), glm::quat(glm::vec3(0.f, 0.f, 0.f)), glm::vec3(2.5f));
	renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::SKYOBJECTS);

	m_objects.push_back(new MapObject("Debris_Near"));
	m_objects[m_objects.size() - 1]->loadMesh("StonePlaneNear.mesh");
	m_objects[m_objects.size() - 1]->setTransform(glm::vec3(0.0f, 100.0f, 0.0f), glm::quat(glm::vec3(0.f, 0.f, 0.f)), glm::vec3(2.f));
	renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::SKYOBJECTS);
}

void PlayState::loadDecor()
{
	Renderer* renderer = Renderer::getInstance();
	switch (m_map)
	{
	case 0:
		m_objects.push_back(new MapObject("Academy_Outer"));
		m_objects[m_objects.size() - 1]->loadMesh("ExteriorTest.mesh");
		renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::STATIC);
		break;

	case 1:
		break;

	default:
		break;

	}
}

void PlayState::loadLights()
{
	Renderer* renderer = Renderer::getInstance();
	switch (m_map)
	{
	case 0:
		// Church
		m_pointlights.emplace_back(new Pointlight(glm::vec3(49.0f, 15.0f, 2.0f), glm::vec3(0.3, 0.85, 1.0), 2.5f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 65.0f));

		// Middle
		m_pointlights.emplace_back(new Pointlight(glm::vec3(0.0f, 24.0f, 0.0f), glm::vec3(0.9, 0.17, 0.123), 8.8f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.14f, 0.07f, 47.0f));

		// Court area
		m_pointlights.emplace_back(new Pointlight(glm::vec3(-41.0f, 21.0f, 10.0f), glm::vec3(0.9, 0.2, 0.5), 2.5f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.045f, 0.0075f, 100.0f));

		// Back wall platforms M
		m_pointlights.emplace_back(new Pointlight(glm::vec3(-2.0f, 19.0f, -31.0f), glm::vec3(0.98, 0.675, 0.084), 8.8f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.14f, 0.11f, 47.0f));

		// Back wall platforms R
		m_pointlights.emplace_back(new Pointlight(glm::vec3(-31.0f, 17.0f, -37.0f), glm::vec3(0.98, 0.675, 0.084), 8.8f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.14, 0.11f, 47.0f));

		// Back wall platforms L
		m_pointlights.emplace_back(new Pointlight(glm::vec3(29.0f, 19.0f, -37.0f), glm::vec3(0.98, 0.675, 0.084), 8.8f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.14f, 0.11f, 47.0f));

		// Maze
		m_pointlights.emplace_back(new Pointlight(glm::vec3(-100.0f, 13.0f, -4.0f), glm::vec3(0.9, 0.9, 1.0), 2.5f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.09f, 0.032f, 64.0f));
		break;

	case 1:
		// Light Middle
		m_pointlights.emplace_back(new Pointlight(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(1.0, 1.0, 1.0), 8.8f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 100.0f));

		// Light Right Back
		m_pointlights.emplace_back(new Pointlight(glm::vec3(50.0f, 20.0f, 50.0f), glm::vec3(1.0, 0.0, 1.0), 8.8f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 100.0f));

		// Light Right Forward
		m_pointlights.emplace_back(new Pointlight(glm::vec3(50.0f, 20.0f, -50.0f), glm::vec3(0.0, 1.0, 1.0), 8.8f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 100.0f));

		// Light Left Back
		m_pointlights.emplace_back(new Pointlight(glm::vec3(-50.0f, 20.0f, 50.0f), glm::vec3(0.0, 0.0, 1.0), 8.8f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 100.0f));

		// Light Left Forward
		m_pointlights.emplace_back(new Pointlight(glm::vec3(-50.0f, 20.0f, -50.0f), glm::vec3(0.0, 1.0, 1.0), 8.8f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 100.0f));
		break;



	default:
		break;
	}

	for (Pointlight* p : m_pointlights)
	{
		renderer->submit(p, RENDER_TYPE::POINTLIGHT_SOURCE);
	}
}

// Might change these Pepega constructors later if feeling cute
void PlayState::loadDestructables()
{
	m_dstr = DstrGenerator();
	m_dstr_alt1 = DstrGenerator();
	Renderer* renderer = Renderer::getInstance();
	BGLoader meshLoader; // The file loader

	// Temporary variables to move later ---
	// Debug Destructibles
	int breakPoints = 11;
	float breakRadius = 2.8f;

	/*float gravityOnImpact = 0.0f;
	float timeToChange = 2.0f;
	float gravityAfterTime = 30.0f;*/ // Stop-fall effect

	float gravityOnImpact = -1.0f;
	float timeToChange = 2.5f;
	float gravityAfterTime = -8.0f; // Hover-up effect

	// Temporary variables to move later ---
	m_dstr.setBreakSettings(DSTR2, 11, 2.8f, gravityOnImpact);
	m_dstr_alt1.setBreakSettings(DSTR1, 8, 3.4f, gravityOnImpact);

	switch (m_map)
	{
	case 0:


		for (int i = (int)m_objects.size() - 1; i >= 0; i--)
		{
			if (m_objects[i]->getType() == DESTRUCTIBLE)
			{
				renderer->removeRenderObject(m_objects[i], STATIC);
				delete m_objects[i];
				m_objects.erase(m_objects.begin() + i);
			}
		}

		// Wall desctructibles
		meshLoader.LoadMesh(MESHPATH + "Towermap/DSTRWalls.mesh");
		for (int i = 0; i < (int)meshLoader.GetMeshCount(); i++)
		{
			m_objects.emplace_back(new DestructibleObject(
				&m_dstr,
				m_objects.size(),
				gravityAfterTime,
				timeToChange));

			static_cast<DestructibleObject*>(m_objects.back())->loadDestructible(
				meshLoader.GetVertices(i),
				meshLoader.GetMeshName(i),
				meshLoader.GetMaterial(i),
				meshLoader.GetAlbedo(i),
				meshLoader.GetTransform(i),
				0.15f
			);

			m_objects.back()->makeStatic();
			Renderer::getInstance()->submit(m_objects.back(), STATIC);
		}
		meshLoader.Unload();

		// Maze desctructibles
		meshLoader.LoadMesh(MESHPATH + "DSTRMaze.mesh");
		for (int i = 0; i < (int)meshLoader.GetMeshCount(); i++)
		{
			m_objects.emplace_back(new DestructibleObject(
				&m_dstr,
				m_objects.size(),
				gravityAfterTime,
				timeToChange
			));

			static_cast<DestructibleObject*>(m_objects.back())->loadDestructible(
				meshLoader.GetVertices(i),
				meshLoader.GetMeshName(i),
				meshLoader.GetMaterial(i),
				meshLoader.GetAlbedo(i),
				meshLoader.GetTransform(i),
				0.25f
			);

			m_objects.back()->makeStatic();
			Renderer::getInstance()->submit(m_objects.back(), STATIC);
		}
		meshLoader.Unload();

		// Pillar destructibles
		meshLoader.LoadMesh(MESHPATH + "Towermap/DSTRPillars.mesh");
		for (int i = 0; i < (int)meshLoader.GetMeshCount(); i++)
		{
			m_objects.emplace_back(new DestructibleObject(
				&m_dstr_alt1,
				m_objects.size(),
				gravityAfterTime,
				timeToChange
			));

			static_cast<DestructibleObject*>(m_objects.back())->loadDestructible(
				meshLoader.GetVertices(i),
				meshLoader.GetMeshName(i),
				meshLoader.GetMaterial(i),
				meshLoader.GetAlbedo(i),
				meshLoader.GetTransform(i),
				1.0f
			);

			m_objects.back()->makeStatic();
			Renderer::getInstance()->submit(m_objects.back(), STATIC);
		}
		meshLoader.Unload();

		// CONCEPT
		//// Outside walls destructibles
		//meshLoader.LoadMesh(MESHPATH + "DSTROutsideWalls.mesh");
		//for (int i = 0; i < (int)meshLoader.GetMeshCount(); i++)
		//{
		//	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
		//
		//	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible(
		//		meshLoader.GetVertices(i),
		//		meshLoader.GetMeshName(i),
		//		meshLoader.GetMaterial(i),
		//		meshLoader.GetAlbedo(i),
		//		meshLoader.GetTransform(i),
		//		1.6f
		//	);
		//
		//	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
		//	Renderer::getInstance()->submit(m_objects.back(), STATIC);
		//}
		//meshLoader.Unload();
		break;

		case 1:
			// Debug Destructibles
			breakPoints = 11;
			breakRadius = 2.8f;

			gravityOnImpact = 30.0f;
			timeToChange = 2.5f;
			gravityAfterTime = 30.0f; // Hover-up effect

			// Temporary variables to move later ---
			m_dstr.setBreakSettings(DSTR2, breakPoints, breakRadius, gravityOnImpact);

			meshLoader.LoadMesh(MESHPATH + "Debug_DSTR.mesh");
			for (int i = 0; i < (int)meshLoader.GetMeshCount(); i++)
			{
				m_objects.emplace_back(new DestructibleObject(
					&m_dstr,
					m_objects.size(),
					gravityAfterTime,
					timeToChange));

				static_cast<DestructibleObject*>(m_objects.back())->loadDestructible(
					meshLoader.GetVertices(i),
					meshLoader.GetMeshName(i),
					meshLoader.GetMaterial(i),
					meshLoader.GetAlbedo(i),
					meshLoader.GetTransform(i),
					0.15f
				);

				m_objects.back()->makeStatic();
				Renderer::getInstance()->submit(m_objects.back(), STATIC);
			}
			meshLoader.Unload();

		default:
			break;
	}
}

PlayState::~PlayState()
{
	mu.printBoth("Before deleting playstate:");

	for (GameObject* object : m_objects)
		delete object;

	for (Pointlight* light : m_pointlights)
		if (light)
			delete light;

	if (deathBuffer)
		delete deathBuffer;

	GUIclear();

	m_pointlights.clear();
	m_objects.clear();

	delete m_skybox;
	delete m_player;
	delete m_spellHandler;
	delete m_camera;
	delete m_firstPerson;
	if (LocalServer::getInstance()->isInitialized()) {
		LocalServer::getInstance()->destroy();
	}

	if (Client::getInstance()->isInitialized()) {
		Client::getInstance()->destroy();
	}

	BulletPhysics::getInstance()->destroy();
	MeshMap::getInstance()->cleanUp();

	TextRenderer::getInstance()->cleanup();
	TextManager::getInstance()->cleanup();

	mu.printBoth("Afer deleting playstate:");
	
}


static float memTimer = 0.0f;
static float fpsTimer = 0.0f;
void PlayState::update(float dt)
{
	Client::getInstance()->updateNetworkEntities(dt);
	auto* clientPtr = Client::getInstance();
	m_dstr.update();
	m_dstr_alt1.update();
	if (clientPtr->isSpectating()) {
		update_isSpectating(dt);
	}
	else{
		update_isPlaying(dt);
	}

	removeDeadObjects();

#if SHOW_MEMORY_INFO
	memTimer += DeltaTime;
	if (memTimer >= 0.5f) {
		memTimer = 0.0f;
		mu.updateBoth();
		std::string memTex = "Ram: " + std::to_string(mu.getCurrentRamUsage()) + " | VRam: " + std::to_string(mu.getCurrentVramUsage()) +
			" | Highest Ram: " + std::to_string(mu.getHighestRamUsage()) + " | Highest VRam: " + std::to_string(mu.getHighestVramUsage());
		m_memoryText->changeText(memTex);
	}
#endif

	fpsTimer += DeltaTime;
	if (fpsTimer >= 1.0f) {
		fpsTimer = 0.0f;
		m_fpsText->changeText("fps: " + std::to_string(Framerate));
	}

	TextManager::getInstance()->update();
}

void PlayState::removeDeadObjects()
{
	Renderer* renderer = Renderer::getInstance();
	for (int i = (int)m_objects.size() - 1; i >= 0; i--)
	{
		// Destructible case
		if (m_objects[i]->getType() == DESTRUCTIBLE)
		{
			DestructibleObject* obj = static_cast<DestructibleObject*>(m_objects[i]);
			if (obj->is_destroyed() && obj->getLifetime() >= 20.0 )
			{
				renderer->removeRenderObject(m_objects[i], STATIC);
				// Keeping gameobjects for now, desync in indexing with server
				//delete m_objects[i];
				//m_objects.erase(m_objects.begin() + i);
			}
		}
	}
}

void PlayState::onSpellHit_callback()
{

}

void PlayState::update_isPlaying(const float& dt)
{
	SoundHandler* shPtr = SoundHandler::getInstance();
	auto* clientPtr = Client::getInstance();
	clientPtr->updateNetworkEntities(dt);

	m_player->update(dt);
	m_spellHandler->spellUpdate(dt);

	BulletPhysics::getInstance()->update(dt);
	Renderer::getInstance()->updateParticles(dt);

	shPtr->setSourcePosition(m_player->getPlayerPos(), HitmarkSound);

	for (Evnt evnt = clientPtr->readNextEvent(); evnt.playerEvent != PlayerEvents::None; evnt = clientPtr->readNextEvent()) {

		switch (evnt.playerEvent) {

			case PlayerEvents::GameCountdown:
			{
				static int lastNumber = 0;
				CountdownPacket pp;
				memcpy(&pp, evnt.data, sizeof(CountdownPacket));

				if (&pp != nullptr) {

					if (lastNumber == (pp.timeLeft / 1000)  || (pp.timeLeft / 1000) == 11){
						delete evnt.data;
						break;
					}

					lastNumber = pp.timeLeft / 1000;

					if (lastNumber == 0) {
						GUIText* t = TextManager::getInstance()->addDynamicText(
							"Game has begun!",
							0.25f,
							glm::vec3(0.0f, -0.25f, 0.0f),
							2.5f,
							TextManager::TextBehaviour::Instant_FadOut,
							glm::vec3(0.0f, 0.0f, 0.0f), true);

						t->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
						t->setScale(1.0f);
					}
					else {
						GUIText* t = TextManager::getInstance()->addDynamicText(
							"Game Starting in: " + std::to_string(lastNumber),
							0.25f,
							glm::vec3(0.0f, -0.25f, 0.0f),
							1.f,
							TextManager::TextBehaviour::Instant_FadOut,
							glm::vec3(0.0f, 0.0f, 0.0f), true);

						t->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
						t->setScale(1.0f);
					}

					



					delete evnt.data;
				}

				break;
			}
			case PlayerEvents::RoundTimer:
			{
				static int lastSecond = 0;
				RoundTimePacket rp;
				memcpy(&rp, evnt.data, sizeof(RoundTimePacket));

				if (&rp != nullptr) {

					if (lastSecond == rp.seconds) {
						delete evnt.data;
						break;
					}

					lastSecond = rp.seconds;

					if (m_gameTimeText == nullptr) {
						m_gameTimeText = TextManager::getInstance()->addDynamicText(
							std::to_string(rp.minutes) + ":" + std::to_string(lastSecond),
							0.25f,
							glm::vec3(0.0f, -0.035f, 0.0f),
							1.f,
							TextManager::TextBehaviour::StayForever,
							glm::vec3(0.0f, 0.0f, 0.0f), true);
					}
					else {

						std::string secondsText = (rp.seconds <= 9 ? "0" : "") + std::to_string(rp.seconds);
						std::string preText = "";
						
						if (Client::getInstance()->getServerState().currentState == NetGlobals::SERVER_STATE::GameFinished)
							preText = "End of round ";

						m_gameTimeText->changeText(preText + std::to_string(rp.minutes) + ":" + secondsText);

					}

					m_gameTimeText->setShouldRender(true);

				}



				delete evnt.data;
				break;
			}
			case PlayerEvents::Died:
			{
				logWarning("[Event system] Died");
				//Update the HP bar
				m_hudHandler.getHudObject(HUDID::BAR_HP)->setXClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100.0f);
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_HP)->setYClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100.0f);
				const PlayerPacket* shooter = clientPtr->getLatestPlayerThatHitMe();
				if (shooter != nullptr) {
					m_lastPositionOfMyKiller = shooter->position;
				}
				m_player->onDead();
				m_camera->disableCameraMovement(true);
				//TODO
				//Here I can implement the poof particles for a dead player

				//m_objects.push_back(new WorldObject("Poof"));
				//m_objects.back()->setWorldPosition(m_player->getPlayerPos());
				//m_objects.back()->addParticle(deathBuffer);
				//m_objects.back()->RemoveParticle(); //Set remove to true, it will now die when the player has respawned
				//Renderer::getInstance()->submit(m_objects.back(), STATIC);


				break;
			}
			case PlayerEvents::Respawned:
			{
				logWarning("[Event system] Respawned");
				//Update the HP bar
				m_player->setPlayerPos(Client::getInstance()->getMyData().latestSpawnPosition);
				m_player->setHealth(NetGlobals::PlayerMaxHealth);
				m_hudHandler.getHudObject(HUDID::BAR_HP)->setXClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100.0f);
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_HP)->setYClip(1.0f);
				m_camera->resetCamera();
				m_camera->disableCameraMovement(false);
				m_player->onRespawn();
				
				for (int i = 0; i < m_objects.size(); i++)
				{
					if (m_objects[i]->ShouldDie()) //Atm this seems to be working fine
					{
						Renderer::getInstance()->removeRenderObject(m_objects[i], STATIC);
						// Might fuck up destruction!!!1
						delete m_objects[i];
						m_objects.erase(m_objects.begin() + i);
					}
				}

				break;
			}
			case PlayerEvents::TookDamage:
			{
				logWarning("[Event system] Took damage");

				shPtr->setSourcePosition(m_player->getPlayerPos(), TakingDamageSound);
				shPtr->playSound(TakingDamageSound);

				const PlayerPacket* shooter = clientPtr->getLatestPlayerThatHitMe();

				if (shooter != nullptr) {
					const glm::vec3& playerPosition = m_player->getPlayerPos();
					const glm::vec3& shooterPosition = shooter->position;
					const glm::vec3& playerRotation = clientPtr->getMyData().rotation; // cause i don't want quaternions..

					glm::vec3 diffVec = shooterPosition - playerPosition;

					float angle = (atan2f(diffVec.x, diffVec.z) * 180.0f) / glm::pi<float>();
					float playerAngle = glm::degrees(playerRotation.y);
					float indicatorAngle = angle - playerAngle;

					// Health
					int myNewHealth = Client::getInstance()->getMyData().health;
					float clipPercentage = static_cast<float>(myNewHealth) / 100.0f;

					// Get all the involved hud objects
					HudObject* DmgIndicator = m_hudHandler.getHudObject(HUDID::DAMAGE_INDICATOR);
					HudObject* DmgOverlay = m_hudHandler.getHudObject(HUDID::DAMAGE_OVERLAY);
					HudObject* HpBar = m_hudHandler.getHudObject(HUDID::BAR_HP);
					m_hudHandler.getHudObject(HUDID::CROSSHAIR_HP)->setYClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100.0f);

					DmgIndicator->setRotation(glm::quat(glm::vec3(0, 0, glm::radians(indicatorAngle))));
					DmgIndicator->setAlpha(1.0f);
					DmgOverlay->setAlpha(1.0f);

					HpBar->setXClip(clipPercentage);
					m_player->setHealth(myNewHealth);
				}

				break;
			}
			case PlayerEvents::Hitmark:
			{
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_HIT)->setAlpha(1.0f);
				shPtr->playSound(HitmarkSound);

				if (evnt.data == nullptr) continue;

				HitConfirmedPacket pp;
				memcpy(&pp, evnt.data, sizeof(HitConfirmedPacket));
				
				if (&pp != nullptr) {
					
					auto txtPos = pp.targetPosition + glm::vec3(0.0f, Client::getInstance()->getMyData().meshHalfSize.y * 2.0f, 0.0f);
					float dist = glm::distance(m_player->getPlayerPos(), txtPos);
					

					GUIText* t = TextManager::getInstance()->addDynamicText(
						std::to_string(pp.damageDone),
						2.0f,
						txtPos,
						2.0f,
						TextManager::TextBehaviour::Instant_FadOut,
						glm::vec3(Randomizer::single(-0.5f, 0.5f), 1.5f, Randomizer::single(-0.5f, 0.5f)), false, 1.0f);

					t->setColor(glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
					t->setScale(fminf(fmaxf(dist / 15.0f, 1.0f), 5.0f));
					t->setToFaceCamera(true);
					t->setIgnoreDepthTest(true);
				}

				delete evnt.data;
				break;
			}
			case PlayerEvents::TookMana:
			{
				shPtr->playSound(PickupSound);
				m_player->setMana(NetGlobals::PlayerMaxMana);
				logWarning("[Event system] Took a mana potion");
				m_hudHandler.getHudObject(HUDID::BAR_MANA)->setXClip(static_cast<float>(Client::getInstance()->getMyData().mana) / 100.0f);
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_MANA)->setYClip(static_cast<float>(Client::getInstance()->getMyData().mana) / 100.0f);
				m_hudHandler.getHudObject(HUDID::MANA_OVERLAY)->setAlpha(0.75f);
				break;
			}
			case PlayerEvents::TookHeal:
			{
				shPtr->playSound(PickupSound);
				logWarning("[Event system] Took a heal");
				m_hudHandler.getHudObject(HUDID::BAR_HP)->setXClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100.0f);
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_HP)->setYClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100.0f);

				HudObject* DmgOverlay = m_hudHandler.getHudObject(HUDID::HEAL_OVERLAY);
				DmgOverlay->setAlpha(1.0f);

				break;
			}
			case PlayerEvents::SessionOver:
			{
				logWarning("[Event system] Session is over");
				HudObject* HpBar = m_hudHandler.getHudObject(HUDID::BAR_HP);
				int myNewHealth = clientPtr->getMyData().health;
				float clipPercentage = 1.0f;
				HpBar->setXClip(clipPercentage);
				m_player->setHealth(myNewHealth);

				m_player->setMana(NetGlobals::PlayerMaxMana);
				m_hudHandler.getHudObject(HUDID::BAR_MANA)->setXClip(static_cast<float>(Client::getInstance()->getMyData().mana) / 100.0f);
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_MANA)->setYClip(static_cast<float>(Client::getInstance()->getMyData().mana) / 100.0f);

				break;
			}
			case PlayerEvents::Deflected:
			{
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_DEFLECT_INDICATOR)->setAlpha(1.0f);
				m_hudHandler.getHudObject(HUDID::MANA_OVERLAY)->setAlpha(0.75f);
				//Give the player a boost in mana
				m_player->increaseMana(10.0f);
				//TODO
				//Implement particles for self deflect here
				break;
			}
			case PlayerEvents::EnemyDeflected:
			{

				break;
			}
			case PlayerEvents::WallGotDestroyed:
			{
				std::lock_guard<std::mutex> lockGuard(NetGlobals::ReadDestructableWallsMutex); // Thread safe

				auto& vec = Client::getInstance()->getDestructedWalls();
				for (size_t i = 0; i < vec.size(); i++) {
					const DestructionPacket& p = vec[i];

					// Destroy
					m_dstr.seedRand(p.randomSeed);
					m_dstr.Destroy(static_cast<DestructibleObject*>(m_objects[p.index]), p.hitPoint, p.hitDir);
				}

				// Tells the client to clear the vector
				Client::getInstance()->clearDestroyedWallsVector();

				break;
			}
			case PlayerEvents::PlayerReady:
			{	

				if (m_readyText == nullptr) {
					m_readyText = TextManager::getInstance()->addDynamicText("Press F1 to ready up!",
						0.20f,
						glm::vec3(0.0f, -1.35f, 0.0f),
						2.5f,
						TextManager::TextBehaviour::StayForeverAndScale,
						glm::vec3(0.0f, 0.0f, 0.0f), true);

					m_readyText->setColor(glm::vec4(1.0f, 0.75f, 0.75f, 1.0f));
					m_readyText->setScale(1.0f);
				}

				if (m_numberOfPlayersReadyText == nullptr) {
					m_numberOfPlayersReadyText = TextManager::getInstance()->addDynamicText(
						"Players ready: " + std::to_string(Client::getInstance()->getNumberOfReadyPlayers()) 
						+ "/" +std::to_string(Client::getInstance()->getNumberOfPlayers()),
						0.15f,
						glm::vec3(0.0f, -0.05f, 0.0f),
						2.5f,
						TextManager::TextBehaviour::StayForever,
						glm::vec3(0.0f, 0.0f, 0.0f), true);

					m_numberOfPlayersReadyText->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					m_numberOfPlayersReadyText->setScale(1.0f);
				}
				else {
					m_numberOfPlayersReadyText->changeText("Players ready: " + std::to_string(Client::getInstance()->getNumberOfReadyPlayers())
						+ "/" + std::to_string(Client::getInstance()->getNumberOfPlayers()));

				}


				break;
			}

			case PlayerEvents::GameStarted:
			{
				m_hudHandler.getHudObject(HUDID::BAR_MANA)->setYClip(1.0f);
				m_hudHandler.getHudObject(HUDID::BAR_HP)->setYClip(1.0f);
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_HP)->setYClip(1.0f);
				loadDestructables();
				break;
			}

			case PlayerEvents::GameIsAboutToStart:
			{
				if (m_gameTimeText != nullptr) {
					m_gameTimeText->setShouldRender(false);
				}

				if (m_numberOfPlayersReadyText != nullptr) {
					m_numberOfPlayersReadyText->setShouldRender(false);
				}

				break;
			}

			case PlayerEvents::WaitingForPlayers:
			{
				if (m_gameTimeText != nullptr) {
					m_gameTimeText->setShouldRender(false);
				}

				if (m_numberOfPlayersReadyText != nullptr) {
					m_numberOfPlayersReadyText->setShouldRender(true);
				}

				if (m_readyText != nullptr) {
					m_readyText->setShouldRender(true);
				}
				break;
			}

			case PlayerEvents::GameEnded:
			{
				m_hudHandler.getHudObject(HUDID::BAR_MANA)->setYClip(1.0f);
				m_hudHandler.getHudObject(HUDID::BAR_HP)->setYClip(1.0f);
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_HP)->setYClip(1.0f);
				break;
			}
			
			case PlayerEvents::EnemyDied:
			{
				if (evnt.data == nullptr) continue;

				for (int i = 0; i < m_objects.size(); i++)
				{
					if (m_objects[i]->ShouldDie()) //Atm this seems to be working fine
					{
						Renderer::getInstance()->removeRenderObject(m_objects[i], STATIC);
						// Might fuck up destruction!!!1
						delete m_objects[i];
						m_objects.erase(m_objects.begin() + i);
					}
				}

				EnemyDiedPacket packet;
				memcpy(&packet, evnt.data, sizeof(EnemyDiedPacket));
				auto* player = Client::getInstance()->findPlayerWithGuid(packet.guidOfDeadPlayer);

				// Do stuff here with player pointer
				
				std::printf("Player died at %f, %f, %f\n", player->position.x, player->position.y, player->position.z);


				m_objects.push_back(new WorldObject("Poof"));
				m_objects.back()->setWorldPosition(glm::vec3(player->position.x, player->position.y, player->position.z));
				m_objects.back()->addParticle(deathBuffer);
				m_objects.back()->RemoveParticle(); //Set remove to true, it will now die when the player has respawned
				Renderer::getInstance()->submit(m_objects.back(), STATIC);
				//-------------


				delete evnt.data;
				break;
			}


		}

	}

	if (clientPtr->getMyData().isReady && m_readyText != nullptr && m_readyText->shouldRender() == true)
	{
		m_readyText->setShouldRender(false);
	}

	// Look at the killer when dead ( If he exist )
	if (!m_camera->isCameraActive() && clientPtr->getMyData().health <= 0)
	{
		const PlayerPacket* myKiller = clientPtr->getLatestPlayerThatHitMe();

		if (myKiller != nullptr) {
			glm::vec3 lookPos = CustomLerp(m_lastPositionOfMyKiller, myKiller->position + glm::vec3(0.0f,myKiller->meshHalfSize.y * 1.75f, 0.0f), DeltaTime);
			m_camera->lookAt(lookPos);

			m_lastPositionOfMyKiller = lookPos;
		}
	}

	// Update game objects
	for (GameObject* object : m_objects)
	{
		object->update(dt);
		object->UpdateParticles(dt);
	}
	Renderer::getInstance()->updateParticles(dt);

	GUIHandler();
	if (!m_hideHUD) {
		HUDHandler();
	}

}


void PlayState::update_isSpectating(const float& dt)
{
	auto* clientPtr = Client::getInstance();
	clientPtr->updateNetworkEntities(dt);
	BulletPhysics::getInstance()->update(dt);

	for (GameObject* object : m_objects)
	{
		if (object != nullptr)
		{
			object->update(dt);
			object->UpdateParticles(dt);
		}
			
	}

	for (Evnt evnt = clientPtr->readNextEvent(); evnt.playerEvent != PlayerEvents::None; evnt = clientPtr->readNextEvent()) {

		switch (evnt.playerEvent) {


		case PlayerEvents::GameCountdown:
		{
			static int lastNumber = 0;
			CountdownPacket pp;
			memcpy(&pp, evnt.data, sizeof(CountdownPacket));

			if (&pp != nullptr) {

				if (lastNumber == (pp.timeLeft / 1000) || (pp.timeLeft / 1000) == 11) {
					delete evnt.data;
					break;
				}

				lastNumber = pp.timeLeft / 1000;

				if (lastNumber == 0) {
					GUIText* t = TextManager::getInstance()->addDynamicText(
						"Game has begun!",
						0.25f,
						glm::vec3(0.0f, -0.25f, 0.0f),
						2.5f,
						TextManager::TextBehaviour::Instant_FadOut,
						glm::vec3(0.0f, 0.0f, 0.0f), true);

					t->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					t->setScale(1.0f);
				}
				else {
					GUIText* t = TextManager::getInstance()->addDynamicText(
						"Game Starting in: " + std::to_string(lastNumber),
						0.25f,
						glm::vec3(0.0f, -0.25f, 0.0f),
						1.f,
						TextManager::TextBehaviour::Instant_FadOut,
						glm::vec3(0.0f, 0.0f, 0.0f), true);

					t->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					t->setScale(1.0f);
				}





				delete evnt.data;
			}

			break;
		}
		case PlayerEvents::RoundTimer:
		{
			static int lastSecond = 0;
			RoundTimePacket rp;
			memcpy(&rp, evnt.data, sizeof(RoundTimePacket));

			if (&rp != nullptr) {

				if (lastSecond == rp.seconds) {
					delete evnt.data;
					break;
				}

				lastSecond = rp.seconds;

				if (m_gameTimeText == nullptr) {
					m_gameTimeText = TextManager::getInstance()->addDynamicText(
						std::to_string(rp.minutes) + ":" + std::to_string(lastSecond),
						0.25f,
						glm::vec3(0.0f, -0.035f, 0.0f),
						1.f,
						TextManager::TextBehaviour::StayForever,
						glm::vec3(0.0f, 0.0f, 0.0f), true);
				}
				else {

					std::string secondsText = (rp.seconds <= 9 ? "0" : "") + std::to_string(rp.seconds);
					std::string preText = "";

					if (Client::getInstance()->getServerState().currentState == NetGlobals::SERVER_STATE::GameFinished)
						preText = "End of round ";

					m_gameTimeText->changeText(preText + std::to_string(rp.minutes) + ":" + secondsText);

				}

				m_gameTimeText->setShouldRender(true);

			}



			delete evnt.data;
			break;
		}
		case PlayerEvents::Died:
		{
			logWarning("[Event system] Died");
			//Update the HP bar
			m_hudHandler.getHudObject(HUDID::BAR_HP)->setXClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100.0f);
			m_hudHandler.getHudObject(HUDID::CROSSHAIR_HP)->setYClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100.0f);
			const PlayerPacket* shooter = clientPtr->getLatestPlayerThatHitMe();
			if (shooter != nullptr) {
				m_lastPositionOfMyKiller = shooter->position;
			}
			m_player->onDead();
			m_camera->disableCameraMovement(true);
			
			break;
		}

		case PlayerEvents::WallGotDestroyed:
		{
			std::lock_guard<std::mutex> lockGuard(NetGlobals::ReadDestructableWallsMutex); // Thread safe

			auto& vec = Client::getInstance()->getDestructedWalls();
			for (size_t i = 0; i < vec.size(); i++) {
				const DestructionPacket& p = vec[i];

				// Destroy
				m_dstr.seedRand(p.randomSeed);
				m_dstr.Destroy(static_cast<DestructibleObject*>(m_objects[p.index]), p.hitPoint, p.hitDir);
			}

			// Tells the client to clear the vector
			Client::getInstance()->clearDestroyedWallsVector();

			break;
		}
		case PlayerEvents::PlayerReady:
		{

			if (m_numberOfPlayersReadyText == nullptr) {
				m_numberOfPlayersReadyText = TextManager::getInstance()->addDynamicText(
					"Players ready: " + std::to_string(Client::getInstance()->getNumberOfReadyPlayers())
					+ "/" + std::to_string(Client::getInstance()->getNumberOfPlayers()),
					0.15f,
					glm::vec3(0.0f, -0.05f, 0.0f),
					2.5f,
					TextManager::TextBehaviour::StayForever,
					glm::vec3(0.0f, 0.0f, 0.0f), true);

				m_numberOfPlayersReadyText->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				m_numberOfPlayersReadyText->setScale(1.0f);
			}
			else {
				m_numberOfPlayersReadyText->changeText("Players ready: " + std::to_string(Client::getInstance()->getNumberOfReadyPlayers())
					+ "/" + std::to_string(Client::getInstance()->getNumberOfPlayers()));

			}


			break;
		}

		case PlayerEvents::GameStarted:
		{
			loadDestructables();
			break;
		}

		case PlayerEvents::GameIsAboutToStart:
		{
			if (m_gameTimeText != nullptr) {
				m_gameTimeText->setShouldRender(false);
			}

			if (m_numberOfPlayersReadyText != nullptr) {
				m_numberOfPlayersReadyText->setShouldRender(false);
			}

			break;
		}

		case PlayerEvents::WaitingForPlayers:
		{
			if (m_gameTimeText != nullptr) {
				m_gameTimeText->setShouldRender(false);
			}

			if (m_numberOfPlayersReadyText != nullptr) {
				m_numberOfPlayersReadyText->setShouldRender(true);
			}

			if (m_readyText != nullptr) {
				m_readyText->setShouldRender(true);
			}
			break;
		}

		case PlayerEvents::EnemyDied:
		{
			if (evnt.data == nullptr) continue;

			for (int i = 0; i < m_objects.size(); i++)
			{
				if (m_objects[i]->ShouldDie()) //Atm this seems to be working fine
				{
					Renderer::getInstance()->removeRenderObject(m_objects[i], STATIC);
					// Might fuck up destruction!!!1
					delete m_objects[i];
					m_objects.erase(m_objects.begin() + i);
				}
			}

			EnemyDiedPacket packet;
			memcpy(&packet, evnt.data, sizeof(EnemyDiedPacket));
			auto* player = Client::getInstance()->findPlayerWithGuid(packet.guidOfDeadPlayer);

			// Do stuff here with player pointer
			m_objects.push_back(new WorldObject("Poof"));
			m_objects.back()->setWorldPosition(glm::vec3(player->position.x, player->position.y, player->position.z));
			m_objects.back()->addParticle(deathBuffer);
			m_objects.back()->RemoveParticle(); //Set remove to true, it will now die when the player has respawned
			Renderer::getInstance()->submit(m_objects.back(), STATIC);
			//-------------

			delete evnt.data;
			break;
		}


		}

	}



	m_camera->update();
	m_spellHandler->spellUpdate(dt);

	Renderer::getInstance()->updateParticles(dt);
	// Update game objects
	for (GameObject* object : m_objects)
	{
		object->update(dt);
		object->UpdateParticles(dt);
	}

	GUIHandler();

	if (Client::getInstance()->isSpectating() == false) {
		if (!m_hideHUD) {
			HUDHandler();
		}
	}
}

void PlayState::render()
{	
	Renderer::getInstance()->render();
}

bool PlayState::callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2)
{
	SoundHandler* shPtr = SoundHandler::getInstance();
	auto* clientPtr = Client::getInstance();
	GameObject* sp1 = static_cast<GameObject*>(obj1->getCollisionObject()->getUserPointer());
	GameObject* sp2 = static_cast<GameObject*>(obj2->getCollisionObject()->getUserPointer());
	if (!sp1 || !sp2)
		return false;	

	DestructibleObject* dstrObj = nullptr;
	Spell* spellObj = nullptr;
	Player* playerObj = nullptr;

	btVector3 hitpoint;

	switch (sp1->getType())
	{
	case (DESTRUCTIBLE):
		dstrObj = static_cast<DestructibleObject*>(sp1);
		hitpoint = cp.m_localPointA;
		break;

	case (NORMALATTACK):
		spellObj = static_cast<Spell*>(sp1);
		break;

	case (ENHANCEATTACK):
		spellObj = static_cast<Spell*>(sp1);
		break;

	case (FLAMESTRIKE):
		spellObj = static_cast<Spell*>(sp1);
		break;
	}

	switch (sp2->getType())
	{
	case (DESTRUCTIBLE):
		dstrObj = static_cast<DestructibleObject*>(sp2);
		hitpoint = cp.m_localPointB;
		break;

	case (NORMALATTACK):
		spellObj = static_cast<Spell*>(sp2);
		break;

	case (ENHANCEATTACK):
		spellObj = static_cast<Spell*>(sp2);
		break;

	case (FLAMESTRIKE):
		spellObj = static_cast<Spell*>(sp2);
		break;
	}

	if (spellObj)
	{
		if (!spellObj->getHasCollided())
			spellObj->hasCollided();
	}

	if (dstrObj && spellObj)
	{
		if (!dstrObj->is_destroyed())
		{
			DstrGenerator* m_dstr = dstrObj->getDstr();
			int seed = m_dstr->seedRand();

			m_dstr->Destroy(dstrObj, glm::vec2(hitpoint.getX(), hitpoint.getY()), spellObj->getDirection());

			// TODO: Fix this
			if (spellObj->getRigidBody() != nullptr)
			{
				float rndX = rand() % 2000 + 1 - 1000; rndX /= 1000;
				float rndY = rand() % 2000 + 1 - 1000; rndY /= 1000;
				float rndZ = rand() % 2000 + 1 - 1000; rndZ /= 1000;
				spellObj->getRigidBody()->setLinearVelocity(btVector3(rndX, rndY, rndZ) * 35);
			}


			//if (spellObj->getType() != FLAMESTRIKE)
			//	spellObj->setTravelTime(0.0f);

			// Network packet
			DestructionPacket dstrPacket;
			dstrPacket.hitPoint = glm::vec2(hitpoint.getX(), hitpoint.getY());
			dstrPacket.hitDir = spellObj->getDirection();
			dstrPacket.index = dstrObj->getIndex();
			dstrPacket.randomSeed = seed;

			Client::getInstance()->sendDestructionPacket(dstrPacket);
		}
	}

	if (dstrObj)
	{
		if (obj1->getCollisionObject()->getCollisionShape()->getName() == "CapsuleZ")
			playerObj = static_cast<Player*>(obj1->getCollisionObject()->getUserPointer());
		else if (obj2->getCollisionObject()->getCollisionShape()->getName() == "CapsuleZ")
			playerObj = static_cast<Player*>(obj2->getCollisionObject()->getUserPointer());
		if (!playerObj)
			return false;

		//apply a force from the player to the object
		for (size_t i = 0; i < dstrObj->getMeshesCount(); i++)
		{
			btVector3 btRigPos = dstrObj->getRigidBody(i)->getCenterOfMassPosition();
			glm::vec3 glmPlayerPos = playerObj->getPlayerPos();
			btVector3 playerPos = btVector3(glmPlayerPos.x, glmPlayerPos.y, glmPlayerPos.z);

			btVector3 dir = btRigPos - playerPos;
			dir.normalize();
			dstrObj->getRigidBody(i)->applyCentralImpulse(dir * 5);
		}
	}


	return false;
}

void PlayState::InitParticle()
{
	PSinfo tempPS;
	TextureInfo tempTxt;

	tempTxt.name = "Assets/Textures/betterSmoke2.png";
	tempPS.width = 0.9f;
	tempPS.heigth = 1.2f;
	tempPS.lifetime = 3.0f;
	tempPS.maxParticles = 300;
	tempPS.emission = 0.02f;
	tempPS.force = -0.54f;
	tempPS.drag = 0.0f;
	tempPS.gravity = -2.2f;
	tempPS.seed = 1;
	tempPS.cont = false;
	tempPS.omnious = true;
	tempPS.spread = 5.0f;
	tempPS.glow = 1.3;
	tempPS.scaleDirection = 0;
	tempPS.swirl = 0;
	tempPS.fade = 1;
	tempPS.randomSpawn = true;
	tempPS.color = glm::vec3(0.3f, 0.8f, 0.0f);
	tempPS.blendColor = glm::vec3(0.0f, 1.0f, 0.0f);
	//tempPS.color = glm::vec3(0.0, 0.0f, 0.0f);
	tempPS.direction = glm::vec3(0.0f, -1.0f, 0.0f);

	deathBuffer = new ParticleBuffers(tempPS, tempTxt);
	deathBuffer->setShader(ShaderMap::getInstance()->getShader(PARTICLES)->getShaderID());
	deathBuffer->bindBuffers();
}

void PlayState::HUDHandler() {

	if (m_player == nullptr)
		return;

	//Mana bar
	m_hudHandler.getHudObject(BAR_MANA)->setXClip(m_player->getMana() / 100.0f);
	m_hudHandler.getHudObject(CROSSHAIR_MANA)->setYClip(m_player->getMana() / 100.0f);

	if (m_player->getAttackCooldown() > 0) {
		m_hudHandler.getHudObject(SPELL_ARCANE)->setGrayscale(m_player->getAttackCooldown() / m_player->getMaxAttackCooldown());
	}
	else {
		m_hudHandler.getHudObject(SPELL_ARCANE)->setGrayscale(0);
	}

	if (m_player->getSpecialCooldown() > 0) {
		m_hudHandler.getHudObject(SPELL_TRIPLE)->setGrayscale(m_player->getSpecialCooldown() / m_player->getMaxSpecialCooldown());
		m_hudHandler.getHudObject(SPELL_FLAMESTRIKE)->setGrayscale(m_player->getSpecialCooldown() / m_player->getMaxSpecialCooldown());
	}
	else {
		m_hudHandler.getHudObject(SPELL_TRIPLE)->setGrayscale(0);
	}
	//If triple spell is active
	if (m_player->currentSpell() == ENHANCEATTACK) {	// TODO: Make this dymanic, not hardcoded for enhance and flamestrike
		m_hudHandler.getHudObject(SPELL_TRIPLE)->setAlpha(1.0f);
		m_hudHandler.getHudObject(SPELL_FLAMESTRIKE)->setAlpha(0.0f);
	}
	else {
		m_hudHandler.getHudObject(SPELL_TRIPLE)->setAlpha(0.0f);
		m_hudHandler.getHudObject(SPELL_FLAMESTRIKE)->setAlpha(1.0f);
	}

	//Deflect
	if (m_player->isDeflecting()) {
		m_hudHandler.getHudObject(CROSSHAIR)->setAlpha(0.0f);
		m_hudHandler.getHudObject(CROSSHAIR_DEFLECT)->setAlpha(1.0f);
		m_hudHandler.getHudObject(SPELL_DEFLECT)->setGrayscale(1);
	}
	else {
		m_hudHandler.getHudObject(SPELL_DEFLECT)->setGrayscale(0);
		m_hudHandler.getHudObject(CROSSHAIR)->setAlpha(1.0f);
		m_hudHandler.getHudObject(CROSSHAIR_DEFLECT)->setAlpha(0.0f);
	}

	//Damage Overlay
	if (m_hudHandler.getHudObject(DAMAGE_OVERLAY)->getAlpha() != 0)
	{
		m_hudHandler.getHudObject(DAMAGE_OVERLAY)->setAlpha(m_hudHandler.getHudObject(DAMAGE_OVERLAY)->getAlpha() - DeltaTime);
	}

	if (m_hudHandler.getHudObject(MANA_OVERLAY)->getAlpha() != 0)
	{
		m_hudHandler.getHudObject(MANA_OVERLAY)->setAlpha(m_hudHandler.getHudObject(MANA_OVERLAY)->getAlpha() - DeltaTime);
	}


	if (m_hudHandler.getHudObject(HEAL_OVERLAY)->getAlpha() != 0)
	{
		m_hudHandler.getHudObject(HEAL_OVERLAY)->setAlpha(m_hudHandler.getHudObject(HEAL_OVERLAY)->getAlpha() - DeltaTime);
	}

	if (m_hudHandler.getHudObject(CROSSHAIR_DEFLECT_INDICATOR)->getAlpha() != 0)
	{
		m_hudHandler.getHudObject(CROSSHAIR_DEFLECT_INDICATOR)->setAlpha(m_hudHandler.getHudObject(CROSSHAIR_DEFLECT_INDICATOR)->getAlpha() - DeltaTime);
	}
	//Hitmarker
	if (m_hudHandler.getHudObject(CROSSHAIR_HIT)->getAlpha() > 0.0f) {
		m_hudHandler.getHudObject(CROSSHAIR_HIT)->setAlpha(m_hudHandler.getHudObject(CROSSHAIR_HIT)->getAlpha() - DeltaTime);

		if (m_hudHandler.getHudObject(CROSSHAIR_HIT)->getAlpha() < 0.0f) {
			m_hudHandler.getHudObject(CROSSHAIR_HIT)->setAlpha(0.0f);
		}
	}

	// Damage indicator
	if (m_hudHandler.getHudObject(DAMAGE_INDICATOR)->getAlpha() > 0.0f) {
		m_hudHandler.getHudObject(DAMAGE_INDICATOR)->setAlpha(m_hudHandler.getHudObject(DAMAGE_INDICATOR)->getAlpha() - DeltaTime);

		if (m_hudHandler.getHudObject(DAMAGE_INDICATOR)->getAlpha() < 0.0f) {
			m_hudHandler.getHudObject(DAMAGE_INDICATOR)->setAlpha(0.0f);
		}
	}
}

void PlayState::GUIHandler()
{
	//Open the menu
	if (Input::isKeyPressed(GLFW_KEY_ESCAPE)) {

		SoundHandler::getInstance()->stopSound(DeflectSound, Client::getInstance()->getMyData().guid);

		m_GUIOpen = !m_GUIOpen;
		if (m_GUIOpen) {
			glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_camera->enableFP(false);
			//m_player->logicStop(true);
			GUILoadButtons();
		}
		else {
			glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			m_camera->enableFP(true); //Resets the mouse position as well
			//m_player->logicStop(false);
			GUIclear();
		}
	}

	if (Client::getInstance()->getServerState().currentState != NetGlobals::GameFinished && m_endGameBoardVisible) {
		GUIclear();
		m_endGameBoardVisible = false;
	}

	if (Input::isKeyPressed(GLFW_KEY_TAB) && !m_endGameBoardVisible) {
		GUILoadScoreboard();
	}
	else if (Client::getInstance()->getServerState().currentState == NetGlobals::GameFinished && !m_endGameBoardVisible && !Client::getInstance()->isSpectating()) {
		GUILoadScoreboard();
		m_endGameBoardVisible = true;
	}

	if (Input::isKeyReleased(GLFW_KEY_TAB) && !m_endGameBoardVisible) {
		GUIclear();
	}
}

void PlayState::GUILoadScoreboard() {
	if (!m_scoreboardExists) {
		//Create the scoreboard
		m_scoreBoard = static_cast<CEGUI::MultiColumnList*>(Gui::getInstance()->createWidget(PLAYSECTION, CEGUI_TYPE + "/MultiColumnList", glm::vec4(0.20f, 0.25f, 0.60f, 0.40f), glm::vec4(0.0f), "Scoreboard"));

		m_scoreBoard->addColumn("Players: ", 0, CEGUI::UDim(0.33f, 0));
		m_scoreBoard->addColumn("Kills: ", 1, CEGUI::UDim(0.33f, 0));
		m_scoreBoard->addColumn("Deaths: ", 2, CEGUI::UDim(0.34f, 0));

		int index = 0;

		//Add the client if he's not a spectator
		if (Client::getInstance()->isSpectating() == false) {
			m_scoreBoard->addRow();
			CEGUI::ListboxTextItem* itemMultiColumnList;

			itemMultiColumnList = new CEGUI::ListboxTextItem(Client::getInstance()->getMyData().userName);
			itemMultiColumnList->setSelectionBrushImage(CEGUI_TYPE + "Images" + "/GenericBrush");
			m_scoreBoard->setItem(itemMultiColumnList, 0, static_cast<CEGUI::uint>(index)); // ColumnID, RowID
			itemMultiColumnList = new CEGUI::ListboxTextItem(std::to_string(Client::getInstance()->getMyData().numberOfKills));
			m_scoreBoard->setItem(itemMultiColumnList, 1, static_cast<CEGUI::uint>(index)); // ColumnID, RowID
			itemMultiColumnList = new CEGUI::ListboxTextItem(std::to_string(Client::getInstance()->getMyData().numberOfDeaths));
			m_scoreBoard->setItem(itemMultiColumnList, 2, static_cast<CEGUI::uint>(index)); // ColumnID, RowID
			index++;
		}


		//Add other players
		auto& list = Client::getInstance()->getNetworkPlayersREF().getPlayersREF();

		for (size_t i = 0; i < list.size(); i++)
		{
			m_scoreBoard->addRow();
			CEGUI::ListboxTextItem* itemMultiColumnList;
			itemMultiColumnList = new CEGUI::ListboxTextItem(list.at(i).data.userName);
			itemMultiColumnList->setSelectionBrushImage(CEGUI_TYPE + "Images" + "/GenericBrush");
			m_scoreBoard->setItem(itemMultiColumnList, 0, static_cast<CEGUI::uint>(index)); // ColumnID, RowID

			itemMultiColumnList = new CEGUI::ListboxTextItem(std::to_string(list.at(i).data.numberOfKills));
			m_scoreBoard->setItem(itemMultiColumnList, 1, static_cast<CEGUI::uint>(index)); // ColumnID, RowID,
			itemMultiColumnList = new CEGUI::ListboxTextItem(std::to_string(list.at(i).data.numberOfDeaths));
			m_scoreBoard->setItem(itemMultiColumnList, 2, static_cast<CEGUI::uint>(index)); // ColumnID, RowID
			index++;
		}
		m_scoreboardExists = true;
		m_scoreBoard->setSortColumnByID(1);
		m_scoreBoard->setSortDirection(CEGUI::ListHeaderSegment::SortDirection::Descending);
	}
}

void PlayState::GUILoadButtons()
{
	m_mainMenu = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(PLAYSECTION, CEGUI_TYPE + "/Button", glm::vec4(0.45f, 0.45f, 0.1f, 0.05f), glm::vec4(0.0f), "Exit To Main Menu"));
	m_mainMenu->setText("Main Menu");
	m_mainMenu->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PlayState::onMainMenuClick, this));

	m_quit = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(PLAYSECTION, CEGUI_TYPE + "/Button", glm::vec4(0.45f, 0.55f, 0.1f, 0.05f), glm::vec4(0.0f), "QUIT"));
	m_quit->setText("Quit");
	m_quit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PlayState::onQuitClick, this));
}

void PlayState::GUIclear()
{
	Gui::getInstance()->clearWidgetsInSection(PLAYSECTION);
	m_scoreboardExists = false;
}

bool PlayState::onMainMenuClick(const CEGUI::EventArgs& e)
{
	Renderer::getInstance()->clear();
	m_stateManager->clearAllAndSetState(new MenuState());
	if(SoundHandler::getInstance()->loadSound(ThemeSong0) != -1)
		SoundHandler::getInstance()->playSound(ThemeSong0);
	return true;
}

bool PlayState::onQuitClick(const CEGUI::EventArgs& e) {
	Renderer::getInstance()->clear();
	glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
	return true;
}
