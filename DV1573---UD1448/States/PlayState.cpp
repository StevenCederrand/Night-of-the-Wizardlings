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

	if (spectator == false) {
		
		// To get the height of the character at start due to bounding box calculations.
		GameObject* AnimationMesh = new WorldObject("AnimationMesh");
		AnimationMesh->loadMesh("ANIM.mesh");
		delete AnimationMesh;

		m_spellHandler = new SpellHandler(m_bPhysics);
		m_spellHandler->setOnHitCallback(std::bind(&PlayState::onSpellHit_callback, this));
		
		m_player = new Player(m_bPhysics, "Player", NetGlobals::PlayerFirstSpawnPoint, m_camera, m_spellHandler);
		m_player->setHealth(NetGlobals::PlayerMaxHealth);

		if (Client::getInstance()->isInitialized())
			Client::getInstance()->assignSpellHandler(m_spellHandler);

		m_hudHandler.loadPlayStateHUD();
	}
	else {
		m_spellHandler = new SpellHandler(nullptr);
		m_spellHandler->setOnHitCallback(std::bind(&PlayState::onSpellHit_callback, this));
		m_camera->setSpectatorMode(SpectatorMode::FreeCamera);
	}

	
	Renderer* renderer = Renderer::getInstance();
	renderer->setupCamera(m_camera);

	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();

	renderer->submitSkybox(m_skybox);
	renderer->submitSpellhandler(m_spellHandler);

	m_objects.push_back(new MapObject("Academy_Map"));
	m_objects[m_objects.size() - 1]->loadMesh("Academy.mesh");
	renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::STATIC);

	//Create a pointlight
	Pointlight* pointLight = new Pointlight(glm::vec3(10.0f, 13.0f, 6.0f), glm::vec3(1));

	m_pointlights.emplace_back(pointLight);
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

	// Non dynamic mesh (no rigidbody)
	// Very big mesh hope not overload gpu XD
	m_objects.push_back(new MapObject("Academy_Outer"));
	m_objects[m_objects.size() - 1]->loadMesh("AcademyOuter.mesh");
	renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::STATIC);

	startY = SCREEN_HEIGHT / 2;

	// Destuction
	loadDestructables();

	if(Client::getInstance()->isInitialized())
		Client::getInstance()->assignSpellHandler(m_spellHandler);

	//m_hudHandler.loadPlayStateHUD();
	m_hideHUD = false;	
}

// TODO: loader function for this XD
void PlayState::loadDestructables()
{
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

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall1.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall2.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall3.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall4.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall5.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall6.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall7.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall8.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall9.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall10.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall11.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall12.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall13.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall14.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall15.mesh", 0.15f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTMazeWall1.mesh", 0.25f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTMazeWall2.mesh", 0.25f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTMazeWall3.mesh", 0.25f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTMazeWall4.mesh", 0.25f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTMazeWall5.mesh", 0.25f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTPillar1.mesh", 1.00f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTPillar2.mesh", 1.00f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTPillar3.mesh", 1.00f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_dstr, m_objects.size()));
	static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTPillar4.mesh", 1.00f);
	m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	Renderer::getInstance()->submit(m_objects.back(), STATIC);

	
}

PlayState::~PlayState()
{
	for (GameObject* object : m_objects)
		delete object;

	for (Pointlight* light : m_pointlights)
		delete light;

	GUIclear();

	m_pointlights.clear();
	m_objects.clear();
	
	delete m_skybox;
	delete m_player;
	delete m_bPhysics;
	delete m_spellHandler;
	delete m_camera;
	delete m_deflectBox;

	if (LocalServer::getInstance()->isInitialized()) {
		LocalServer::getInstance()->destroy();
	}

	if (Client::getInstance()->isInitialized()) {
		Client::getInstance()->destroy();
	}

}

void PlayState::update(float dt)
{	
	//m_firstPerson->playLoopAnimation("Test");
	//m_firstPerson->update(dt);
	Client::getInstance()->updateNetworkEntities(dt);
	auto* clientPtr = Client::getInstance();
	m_dstr.update();
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
				delete m_objects[i];
				m_objects.erase(m_objects.begin() + i);
			}
		}
	}
}

void PlayState::onSpellHit_callback()
{
	m_hudHandler.getHudObject(HUDID::CROSSHAIR_HIT)->setAlpha(1.0f);
	SoundHandler::getInstance()->playSound(HitmarkSound);
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
				m_hudHandler.getHudObject(HUDID::BAR_HP)->setXClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100);
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_HP)->setYClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100);
				const PlayerPacket* shooter = clientPtr->getLatestPlayerThatHitMe();
				if (shooter != nullptr) {
					m_lastPositionOfMyKiller = shooter->position;
				}

				m_camera->disableCameraMovement(true);
				break;
			}

			case PlayerEvents::Respawned:
			{
				logWarning("[Event system] Respawned");
				//Update the HP bar 
				m_player->setPlayerPos(Client::getInstance()->getMyData().latestSpawnPosition);
				m_player->setHealth(NetGlobals::PlayerMaxHealth);
				m_hudHandler.getHudObject(HUDID::BAR_HP)->setXClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100);
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_HP)->setYClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100);
				m_camera->resetCamera();
				m_camera->disableCameraMovement(false);
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

					DmgIndicator->setRotation(glm::quat(glm::vec3(0, 0, glm::radians(indicatorAngle))));
					DmgIndicator->setAlpha(1.0f);
					DmgOverlay->setAlpha(1.0f);

					HpBar->setXClip(clipPercentage);
					m_player->setHealth(myNewHealth);
				}

				break;
			}

			case PlayerEvents::TookPowerup:
			{
				logWarning("[Event system] Took a powerup");
				m_hudHandler.getHudObject(HUDID::POWERUP)->setAlpha(1.0f);
				m_hudHandler.getHudObject(HUDID::BAR_HP)->setXClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100);
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_HP)->setYClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100);
				break;
			}

			case PlayerEvents::TookHeal:
			{
				logWarning("[Event system] Took a heal");
				m_hudHandler.getHudObject(HUDID::BAR_HP)->setXClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100);
				m_hudHandler.getHudObject(HUDID::CROSSHAIR_HP)->setYClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100);
				break;
			}
			case PlayerEvents::PowerupRemoved:
			{
				logWarning("[Event system] Powerup was removed");
				m_hudHandler.getHudObject(HUDID::POWERUP)->setAlpha(0.0f);
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
				
				SoundHandler::getInstance()->playSound(DestructionSound, Client::getInstance()->getMyData().guid);
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
	Renderer::getInstance()->render(m_deflectBox);
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
		DstrGenerator* m_dstr = dstrobj->getDstr();
		unsigned int seed = m_dstr->seedRand();		
	
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
		//logTrace(std::to_string(m_player->getSpecialCooldown() / m_player->getMaxSpecialCooldown()));
		m_hudHandler.getHudObject(SPELL_SPECIAL)->setGrayscale(m_player->getSpecialCooldown() / m_player->getMaxSpecialCooldown());
	} 
	else {
		m_hudHandler.getHudObject(SPELL_SPECIAL)->setGrayscale(0);
	}

	if (m_player->isDeflecting()) {
		m_hudHandler.getHudObject(SPELL_DEFLECT)->setGrayscale(1);
	}
	else {
		m_hudHandler.getHudObject(SPELL_DEFLECT)->setGrayscale(0);
	}
		
	//Deflect
	if (m_player->isDeflecting()) {
		m_hudHandler.getHudObject(CROSSHAIR)->setAlpha(0.0f);
		m_hudHandler.getHudObject(CROSSHAIR_DEFLECT)->setAlpha(1.0f);
	}
	else
	{
		m_hudHandler.getHudObject(CROSSHAIR)->setAlpha(1.0f);
		m_hudHandler.getHudObject(CROSSHAIR_DEFLECT)->setAlpha(0.0f);
	}

	//Damage Overlay
	if (m_hudHandler.getHudObject(DAMAGE_OVERLAY)->getAlpha() != 0)
	{
		m_hudHandler.getHudObject(DAMAGE_OVERLAY)->setAlpha(m_hudHandler.getHudObject(DAMAGE_OVERLAY)->getAlpha() - DeltaTime);
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
