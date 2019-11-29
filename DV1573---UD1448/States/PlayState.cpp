#include <Pch/Pch.h>
#include "PlayState.h"
#include <System/StateManager.h>
#include "MenuState.h"
#include <Networking/Client.h>
#include <Networking/LocalServer.h>


#define PLAYSECTION "PLAYSTATE"


void logVec3(glm::vec3 vector) {
	logTrace("Vector: ({0}, {1}, {2})", std::to_string(vector.x), std::to_string(vector.y), std::to_string(vector.z));
}

PlayState::PlayState(bool spectator)
{

	ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setInt("albedoTexture", 0);

	m_camera = new Camera();
	m_bPhysics = new BulletPhysics(-20.0f);

	mu.printBoth("After physics and camera init:");

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

		m_spellHandler = new SpellHandler(m_bPhysics);
		m_spellHandler->setOnHitCallback(std::bind(&PlayState::onSpellHit_callback, this));

		m_player = new Player(m_bPhysics, "Player", NetGlobals::PlayerFirstSpawnPoint, m_camera, m_spellHandler);
		m_player->setHealth(NetGlobals::PlayerMaxHealth);

		if (Client::getInstance()->isInitialized())
			Client::getInstance()->assignSpellHandler(m_spellHandler);

		mu.printBoth("After fps shield, enemy shield and animationMesh:");

		m_hudHandler.loadPlayStateHUD();

		mu.printBoth("After hud:");
	}
	else {
		m_spellHandler = new SpellHandler(nullptr);
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

	m_objects.push_back(new MapObject("Academy_Map"));
	m_objects[m_objects.size() - 1]->loadMesh("Towermap/Academy_t.mesh");
	renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::STATIC);
	mu.printBoth("After Academy map:");
	//			TOO LAGGY ATM
	//LIGHTS
	
	// Church
	Pointlight* pointLight1 = new Pointlight(glm::vec3(49.0f, 15.0f, 2.0f), glm::vec3(0.3, 0.85, 1.0));
	pointLight1->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 65.0f));
	m_pointlights.emplace_back(pointLight1);
	
	// Middle
	Pointlight* pointLight2 = new Pointlight(glm::vec3(0.0f, 24.0f, 0.0f), glm::vec3(0.9, 0.17, 0.123));
	pointLight2->setAttenuationAndRadius(glm::vec4(1.0f, 0.14f, 0.07f, 47.0f));
	m_pointlights.emplace_back(pointLight2);
	
	// Court area
	Pointlight* pointLight3 = new Pointlight(glm::vec3(-41.0f, 21.0f, 10.0f), glm::vec3(0.9, 0.2, 0.5));
	pointLight3->setAttenuationAndRadius(glm::vec4(1.0f, 0.045f, 0.0075f, 100.0f));
	m_pointlights.emplace_back(pointLight3);

	// Back wall platforms M
	Pointlight* pointLight = new Pointlight(glm::vec3(-2.0f, 19.0f, -31.0f), glm::vec3(0.98, 0.675, 0.084));
	pointLight->setAttenuationAndRadius(glm::vec4(1.0f, 0.14f, 0.11f, 47.0f));
	m_pointlights.emplace_back(pointLight);
	
	// Back wall platforms R
	Pointlight* pointLight4 = new Pointlight(glm::vec3(-31.0f, 17.0f, -37.0f), glm::vec3(0.98, 0.675, 0.084));
	pointLight4->setAttenuationAndRadius(glm::vec4(1.0f, 0.14, 0.11f, 47.0f));
	m_pointlights.emplace_back(pointLight4);
	
	// Back wall platforms L
	Pointlight* pointLight5 = new Pointlight(glm::vec3(29.0f, 19.0f, -37.0f), glm::vec3(0.98, 0.675, 0.084));
	pointLight5->setAttenuationAndRadius(glm::vec4(1.0f, 0.14f, 0.11f, 47.0f));
	m_pointlights.emplace_back(pointLight5);
	
	// Maze
	Pointlight* pointLight6 = new Pointlight(glm::vec3(-100.0f, 13.0f, -4.0f), glm::vec3(0.9, 0.9, 1.0));
	pointLight6->setAttenuationAndRadius(glm::vec4(1.0f, 0.09f, 0.032f, 64.0f));
	m_pointlights.emplace_back(pointLight6);
	mu.printBoth("After point lights:");
	// TUNNEL LIGHTS
	//// Tunnel R
	//Pointlight* pointLight7 = new Pointlight(glm::vec3(19.0f, 6.0f, 54.0f), glm::vec3(0.97, 0.377, 0.0));
	//pointLight7->setAttenuationAndRadius(glm::vec4(1.0f, 0.12f, 0.012f, 100.0f));
	//m_pointlights.emplace_back(pointLight7);
	//
	//// Tunnel L
	//Pointlight* pointLight8 = new Pointlight(glm::vec3(-22.0f, 6.0f, 54.0f), glm::vec3(0.97, 0.377, 0.0));
	//pointLight8->setAttenuationAndRadius(glm::vec4(1.0f, 0.12f, 0.012f, 100.0f));
	//m_pointlights.emplace_back(pointLight8);




	for (size_t i = 0; i < m_pointlights.size(); i++)
	{
		renderer->submit(m_pointlights.at(i), RENDER_TYPE::POINTLIGHT_SOURCE);
	}


	//m_firstPerson = new AnimatedObject("NyCharacter");
	//m_firstPerson->loadMesh("NyCharacter.mesh");
	//m_firstPerson->setWorldPosition(glm::vec3(0.0f, 13.0f, 0.0f));
	//m_firstPerson->initAnimations("Test", 2.0f, 109.0f);
	//Renderer::getInstance()->submit(m_firstPerson, ANIMATEDSTATIC);
	//m_firstPerson->playAnimation("Test");

	gContactAddedCallback = callbackFunc;
	// Geneterate bullet objects / hitboxes

	//if (spectator == false) {
		for (size_t i = 0; i < m_objects.size(); i++)
		{
			m_objects.at(i)->createRigidBody(CollisionObject::box, m_bPhysics);
			//m_objects.at(i)->createDebugDrawer();
		}
	//}
		mu.printBoth("After creation of rigidbodies:");
	// Non dynamic mesh (no rigidbody)
	// Very big mesh hope not overload gpu XD
	m_objects.push_back(new MapObject("Academy_Outer"));
	m_objects[m_objects.size() - 1]->loadMesh("ExteriorTest.mesh");
	renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::STATIC);
	mu.printBoth("After Academy Outer:");
	startY = SCREEN_HEIGHT / 2;

	// Destuction 
	loadDestructables();

	mu.printBoth("After destructables:");

	if(Client::getInstance()->isInitialized())
		Client::getInstance()->assignSpellHandler(m_spellHandler);

	//m_hudHandler.loadPlayStateHUD();
	m_hideHUD = false;

	
	mu.printBoth("End of play state init:");

}

// Might change these Pepega constructors later if feeling cute
void PlayState::loadDestructables()
{
	m_dstr = DstrGenerator();
	m_dstr_alt1 = DstrGenerator();


	// Temporary variables to move later ---

	/*float gravityOnImpact = 0.0f;
	float timeToChange = 2.0f;
	float gravityAfterTime = 30.0f;*/ // Stop-fall effect

	float gravityOnImpact = -1.0f;
	float timeToChange = 2.5f;
	float gravityAfterTime = -8.0f; // Hover-up effect

	// Temporary variables to move later ---

	m_dstr.setBreakSettings(DSTR2, 11, 2.8f, gravityOnImpact);
	m_dstr_alt1.setBreakSettings(DSTR1, 8, 3.4f, gravityOnImpact);

	Renderer* renderer = Renderer::getInstance();
	for (int i = (int)m_objects.size() - 1; i >= 0; i--)
	{
		if (m_objects[i]->getType() == DESTRUCTIBLE)
		{
			renderer->removeRenderObject(m_objects[i], STATIC);
			delete m_objects[i];
			m_objects.erase(m_objects.begin() + i);
		}
	}


	BGLoader meshLoader; // The file loader
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

		m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
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

		m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
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

		m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
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
}

PlayState::~PlayState()
{

	mu.printBoth("Before deleting playstate:");

	for (GameObject* object : m_objects)
		delete object;

	for (Pointlight* light : m_pointlights)
		if (light)
			delete light;

	GUIclear();

	m_pointlights.clear();
	m_objects.clear();

	delete m_skybox;
	delete m_player;
	delete m_bPhysics;
	delete m_spellHandler;
	delete m_camera;
	delete m_firstPerson;
	if (LocalServer::getInstance()->isInitialized()) {
		LocalServer::getInstance()->destroy();
	}

	if (Client::getInstance()->isInitialized()) {
		Client::getInstance()->destroy();
	}

	MeshMap::getInstance()->cleanUp();
	
	mu.printBoth("Afer deleting playstate:");

}
	
void PlayState::update(float dt)
{
	//m_firstPerson->playLoopAnimation("Test");
	//m_firstPerson->update(dt);
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

	m_bPhysics->update(dt);
	m_player->update(dt);
	m_spellHandler->spellUpdate(dt);

	Renderer::getInstance()->updateParticles(dt);

	shPtr->setSourcePosition(m_player->getPlayerPos(), HitmarkSound);

	for (PlayerEvents evnt = clientPtr->readNextEvent(); evnt != PlayerEvents::None; evnt = clientPtr->readNextEvent()) {

		switch (evnt) {

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
				break;
			}
			case PlayerEvents::TookMana:
			{
				shPtr->playSound(PickupSound);
				m_player->setMana(NetGlobals::PlayerMaxMana);
				logWarning("[Event system] Took a mana potion");
				m_hudHandler.getHudObject(HUDID::BAR_MANA)->setXClip(static_cast<float>(Client::getInstance()->getMyData().mana) / 100.0f);
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_MANA)->setYClip(static_cast<float>(Client::getInstance()->getMyData().mana) / 100.0f);
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
				// Play sound?
				logTrace("Player ready");

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

			case PlayerEvents::GameEnded:
			{
				m_hudHandler.getHudObject(HUDID::BAR_MANA)->setYClip(1.0f);
				m_hudHandler.getHudObject(HUDID::BAR_HP)->setYClip(1.0f);
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_HP)->setYClip(1.0f);
				break;
			}



		}

	}
	// Look at the killer when dead ( If he exist )
	if (!m_camera->isCameraActive() && clientPtr->getMyData().health <= 0)
	{
		const PlayerPacket* myKiller = clientPtr->getLatestPlayerThatHitMe();

		if (myKiller != nullptr) {
			glm::vec3 lookPos = CustomLerp(m_lastPositionOfMyKiller, myKiller->position + myKiller->meshHalfSize * 1.75f, DeltaTime);
			m_camera->lookAt(lookPos);

			m_lastPositionOfMyKiller = lookPos;
		}
	}

	// Update game objects
	for (GameObject* object : m_objects)
	{
		object->update(dt);
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
	m_bPhysics->update(dt);

	for (GameObject* object : m_objects)
	{
		if(object != nullptr)
			object->update(dt);
	}

	for (PlayerEvents evnt = clientPtr->readNextEvent(); evnt != PlayerEvents::None; evnt = clientPtr->readNextEvent()) {

		switch (evnt) {

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
			// Play sound?
			logTrace("Player ready");

			break;
		}

		case PlayerEvents::GameStarted:
		{
			loadDestructables();
			break;
		}

		case PlayerEvents::GameEnded:
		{

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

	//Renderer::getInstance()->renderDebug();
}

bool PlayState::callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2)
{
	SoundHandler* shPtr = SoundHandler::getInstance();
	auto* clientPtr = Client::getInstance();
	GameObject* sp1 = static_cast<GameObject*>(obj1->getCollisionObject()->getUserPointer());
	GameObject* sp2 = static_cast<GameObject*>(obj2->getCollisionObject()->getUserPointer());
	if (!sp1 || !sp2)
		return false;

	DestructibleObject* dstrobj = nullptr;
	Spell* spellobj = nullptr;

	btVector3 hitpoint;

	switch (sp1->getType())
	{
	case (DESTRUCTIBLE):
		dstrobj = static_cast<DestructibleObject*>(sp1);		
		hitpoint = cp.m_localPointA;
		break;

	case (NORMALATTACK):
		spellobj = static_cast<Spell*>(sp1);
		break;

	case (ENHANCEATTACK):
		spellobj = static_cast<Spell*>(sp1);
		break;

	case (REFLECT):
		spellobj = static_cast<Spell*>(sp1);
		break;

	case (FLAMESTRIKE):
		spellobj = static_cast<Spell*>(sp1);
		break;
	}

	switch (sp2->getType())
	{
	case (DESTRUCTIBLE):
		dstrobj = static_cast<DestructibleObject*>(sp2);
		hitpoint = cp.m_localPointB;
		break;

	case (NORMALATTACK):
		spellobj = static_cast<Spell*>(sp2);
		break;

	case (ENHANCEATTACK):
		spellobj = static_cast<Spell*>(sp2);
		break;

	case (REFLECT):
		spellobj = static_cast<Spell*>(sp2);
		break;

	case (FLAMESTRIKE):
		spellobj = static_cast<Spell*>(sp2);
		break;
	}

	if (spellobj)
	{
		if (!spellobj->getHasCollided())
			spellobj->hasCollided();
	}

	if (dstrobj && spellobj)
	{
		if (!dstrobj->is_destroyed())
		{
			DstrGenerator* m_dstr = dstrobj->getDstr();
			int seed = m_dstr->seedRand();

			m_dstr->Destroy(dstrobj, glm::vec2(hitpoint.getX(), hitpoint.getY()), spellobj->getDirection());

			if (spellobj->getBodyReference() != nullptr)
			{
				float rndX = rand() % 1999 + 1 - 1000; rndX /= 1000;
				float rndY = rand() % 1999 + 1 - 1000; rndY /= 1000;
				float rndZ = rand() % 1999 + 1 - 1000; rndZ /= 1000;
				spellobj->getBodyReference()->setLinearVelocity(btVector3(rndX, rndY, rndZ) * 35);
			}
		
	
			//if (spellobj->getType() != FLAMESTRIKE)
			//	spellobj->setTravelTime(0.0f);

			// Network packet
			DestructionPacket dstrPacket;
			dstrPacket.hitPoint = glm::vec2(hitpoint.getX(), hitpoint.getY());
			dstrPacket.hitDir = spellobj->getDirection();
			dstrPacket.index = dstrobj->getIndex();
			dstrPacket.randomSeed = seed;

			Client::getInstance()->sendDestructionPacket(dstrPacket);
		}
	}


	return false;
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
	if (m_player->usingTripleSpell()) {
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
	SoundHandler::getInstance()->playSound(ThemeSong0);
	return true;
}

bool PlayState::onQuitClick(const CEGUI::EventArgs& e) {
	Renderer::getInstance()->clear();
	glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
	return true;
}
