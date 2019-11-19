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

PlayState::PlayState()
{
	m_bPhysics = new BulletPhysics(-20.0f);
	//to get the right character heigth
	GameObject* AnimationMesh = new WorldObject("AnimationMesh");
	AnimationMesh->loadMesh("ANIM.mesh");
	delete AnimationMesh;

	m_spellHandler = new SpellHandler(m_bPhysics);
	m_spellHandler->setOnHitCallback(std::bind(&PlayState::onSpellHit_callback, this));

	ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setInt("albedoTexture", 0);

	m_camera = new Camera();

	m_player = new Player(m_bPhysics, "Player", NetGlobals::PlayerFirstSpawnPoint, m_camera, m_spellHandler);

	Renderer::getInstance()->setupCamera(m_player->getCamera());

	//TODO: organized loading system?
	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();


	m_player->setHealth(NetGlobals::PlayerMaxHealth);


	m_objects.push_back(new MapObject("Academy_Map"));
	m_objects[m_objects.size() - 1]->loadMesh("Academy.mesh");
	Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);
	

	m_objects.push_back(new WorldObject("Character"));
	m_objects[m_objects.size() - 1]->loadMesh("CharacterTest.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(10.0f, 1.8f, -24.0f));
	Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);

	m_objects.push_back(new Deflect("PlayerShield"));
	m_objects[m_objects.size() - 1]->loadMesh("ShieldMesh.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(10.0f, 13.0f, 6.0f));
	Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], SHIELD);


	
	MaterialMap::getInstance();

	gContactAddedCallback = callbackFunc;
	// Geneterate bullet objects / hitboxes
	for (size_t i = 0; i < m_objects.size(); i++)
	{
		m_objects.at(i)->createRigidBody(CollisionObject::box, m_bPhysics);	
		//m_objects.at(i)->createDebugDrawer();
	}


	// DESTRUCTION TEMP - DESTRUCTION TEMP - DESTRUCTION TEMP - DESTRUCTION TEMP - DESTRUCTION TEMP - DESTRUCTION TEMP
	// Destruction test object
	//m_objects.push_back(new DestructibleObject("Destructible", &m_dstr));
	//static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall1.mesh");
	////m_objects.back()->setWorldPosition(glm::vec3(0.0f, 15.0f, -3.0f));
	//m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	//Renderer::getInstance()->submit(m_objects.back(), STATIC);
	//
	//m_objects.push_back(new DestructibleObject("Destructible2", &m_dstr));
	//static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTWall2.mesh");
	////m_objects.back()->setWorldPosition(glm::vec3(0.0f, 15.0f, 3.0f));
	//m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	//Renderer::getInstance()->submit(m_objects.back(), STATIC);
	//
	//m_objects.push_back(new DestructibleObject("Destructible3", &m_dstr));
	//static_cast<DestructibleObject*>(m_objects.back())->loadDestructible("DSTMazeWall.mesh");
	////m_objects.back()->setWorldPosition(glm::vec3(0.0f, 15.0f, 3.0f), 0);
	//m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	//Renderer::getInstance()->submit(m_objects.back(), STATIC);
	//
	//m_objects.push_back(new DestructibleObject("Destructible4", &m_dstr));
	//static_cast<DestructibleObject*>(m_objects.back())->loadBasic("Dstr_4");
	//m_objects.back()->setWorldPosition(glm::vec3(0.0f, 17.0f, -3.0f), 0);
	//m_objects.back()->createRigidBody(CollisionObject::box, m_bPhysics);
	//Renderer::getInstance()->submit(m_objects.back(), STATIC);

	// DESTRUCTION TEMP - DESTRUCTION TEMP - DESTRUCTION TEMP - DESTRUCTION TEMP - DESTRUCTION TEMP - DESTRUCTION TEMP



	if(Client::getInstance()->isInitialized())
		Client::getInstance()->assignSpellHandler(m_spellHandler);

	m_hudHandler.loadPlayStateHUD();
	m_hideHUD = false;
}

PlayState::~PlayState()
{
	for (GameObject* object : m_objects)
		delete object;
	
	GUIclear();

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
	Client::getInstance()->updateNetworkEntities(dt);
	m_bPhysics->update(dt);
	m_player->update(dt);
	m_spellHandler->spellUpdate(dt);
	Renderer::getInstance()->updateParticles(dt);

	//m_hpBar->setYClip(m_player->getHealth() / 100);
	auto* clientPtr = Client::getInstance();
	for (PlayerEvents evnt = clientPtr->readNextEvent(); evnt != PlayerEvents::None; evnt = clientPtr->readNextEvent()) {

		switch (evnt) {

			case PlayerEvents::Died: 
			{
				logWarning("[Event system] Died");
				//Update the HP bar 
				m_hudHandler.getHudObject(BAR_HP)->setYClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100);
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
				m_hudHandler.getHudObject(BAR_HP)->setYClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100);
				m_camera->resetCamera();
				m_camera->disableCameraMovement(false);
				break;
			}

			case PlayerEvents::TookDamage:
			{
				logWarning("[Event system] Took damage");
			
				const PlayerPacket* shooter = clientPtr->getLatestPlayerThatHitMe();

				if (shooter != nullptr) {
					const glm::vec3& playerPosition = m_player->getPlayerPos();
					const glm::vec3& shooterPosition = shooter->position;
					const glm::vec3& playerRotation = Client::getInstance()->getMyData().rotation; // cause i don't want quaternions..
					
					glm::vec3 diffVec = shooterPosition - playerPosition;

					float angle = (atan2f(diffVec.x, diffVec.z) * 180.0f) / glm::pi<float>();
					float playerAngle = glm::degrees(playerRotation.y);
					float indicatorAngle = angle - playerAngle;

					// Health 
					int myNewHealth = Client::getInstance()->getMyData().health;
					float clipPercentage = static_cast<float>(myNewHealth) / 100.0f;

					// Get all the involved hud objects
					HudObject* DmgIndicator = m_hudHandler.getHudObject(DAMAGE_INDICATOR);
					HudObject* DmgOverlay = m_hudHandler.getHudObject(DAMAGE_OVERLAY);
					HudObject* HpBar = m_hudHandler.getHudObject(BAR_HP);

					DmgIndicator->setRotation(glm::quat(glm::vec3(0, 0, glm::radians(indicatorAngle))));
					DmgIndicator->setAlpha(1.0f);
					DmgOverlay->setAlpha(1.0f);

					HpBar->setYClip(clipPercentage);
					m_player->setHealth(myNewHealth);
					
				}

				
				break;
			}

			case PlayerEvents::TookPowerup:
			{
				logWarning("[Event system] Took a powerup");
				m_hudHandler.getHudObject(POWERUP)->setAlpha(1.0f);
				m_hudHandler.getHudObject(BAR_HP)->setYClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100);
				break;
			}

			case PlayerEvents::TookHeal:
			{
				logWarning("[Event system] Took a heal");
				m_hudHandler.getHudObject(BAR_HP)->setYClip(static_cast<float>(Client::getInstance()->getMyData().health) / 100);
				break;
			}
			case PlayerEvents::PowerupRemoved:
			{
				logWarning("[Event system] Powerup was removed");
				m_hudHandler.getHudObject(POWERUP)->setAlpha(0.0f);
				break;
			}

			case PlayerEvents::SessionOver:
			{
				logWarning("[Event system] Session is over");
				HudObject* HpBar = m_hudHandler.getHudObject(BAR_HP);
				int myNewHealth = Client::getInstance()->getMyData().health;
				float clipPercentage = 1.0f;
				HpBar->setYClip(clipPercentage);
				m_player->setHealth(myNewHealth);
				break;
			}

			case PlayerEvents::WallGotDestroyed:
			{
				// How to send a destruction packet
				// 1. Make one
				// 2. Send it

				/*	Example:
						DestructionPacket p;
						p.index = 0;
						p.hitPoint = glm::vec3(3.0f);
						p.randomSeed = 345342;
						Client::getInstance()->sendDestructionPacket(p);
				*/


				std::lock_guard<std::mutex> lockGuard(NetGlobals::ReadDestructableWallsMutex); // Thread safe
				auto& vec = Client::getInstance()->getDestructedWalls();

				for (size_t i = 0; i < vec.size(); i++) {
					// Do stuff here
					const DestructionPacket& p = vec[i];
					


					//------------------
				}

				// Tells the client to clear the vector
				Client::getInstance()->clearDestroyedWallsVector();

				break;
			}
		}
	}

	// Look at the killer when dead ( If he exist )
	if (!m_camera->isCameraActive() && clientPtr->getMyData().health <= 0)
	{	
		const PlayerPacket* myKiller = clientPtr->getLatestPlayerThatHitMe();

		if (myKiller != nullptr) {
			glm::vec3 lookPos =  CustomLerp(m_lastPositionOfMyKiller, myKiller->position, DeltaTime);
			m_camera->lookAt(lookPos);

			m_lastPositionOfMyKiller = lookPos;
		}
	}

	// Update game objects
	for (GameObject* object : m_objects)
	{
		object->update(dt);
		Renderer::getInstance()->updateParticles(dt);
	}
	
	GUIHandler();
	if (!m_hideHUD) {
		HUDHandler();
	}

}

void PlayState::render()
{	
	Renderer::getInstance()->render(m_skybox, m_deflectBox, m_spellHandler);
	//Renderer::getInstance()->renderDebug();
}

bool PlayState::callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2)
{
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

		float test1 = cp.getAppliedImpulse();
		btVector3 test2 = cp.m_localPointA;
		btVector3 test3 = cp.m_localPointB;
		float test4 = cp.getDistance();
		float test5 = cp.m_contactMotion1;
		float test6 = cp.m_contactMotion2;
		float test7= cp.m_appliedImpulseLateral1;
		float test8 = cp.m_appliedImpulseLateral2;
		btVector3 test9 = cp.m_positionWorldOnA;
		btVector3 test10 = cp.m_positionWorldOnB;

		float test99 = 1;
		
		
		m_dstr->Destroy(dstrobj, glm::vec2(hitpoint.getX(), hitpoint.getY()));
	}


	return false;
}

void PlayState::onSpellHit_callback()
{
	m_hudHandler.getHudObject(CROSSHAIR_HIT)->setAlpha(1.0f);
}

void PlayState::HUDHandler() {
	
	//Mana bar
	m_hudHandler.getHudObject(BAR_MANA)->setYClip(m_player->getMana() / 100.0f);

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
			m_player->logicStop(true);
			GUILoadButtons();
		}
		else {
			glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			m_camera->enableFP(true); //Resets the mouse position as well
			m_player->logicStop(false);
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
	else if (Client::getInstance()->getServerState().currentState == NetGlobals::GameFinished && !m_endGameBoardVisible) {
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
		m_scoreBoard->addColumn("PLAYER: ", 0, CEGUI::UDim(0.33f, 0));
		m_scoreBoard->addColumn("KILLS: ", 1, CEGUI::UDim(0.33f, 0));
		m_scoreBoard->addColumn("DEATHS: ", 2, CEGUI::UDim(0.34f, 0));

		//Add the client
		m_scoreBoard->addRow();
		CEGUI::ListboxTextItem* itemMultiColumnList;

		itemMultiColumnList = new CEGUI::ListboxTextItem(Client::getInstance()->getMyData().userName);
		itemMultiColumnList->setSelectionBrushImage(CEGUI_TYPE + "Images" + "/GenericBrush");
		m_scoreBoard->setItem(itemMultiColumnList, 0, static_cast<CEGUI::uint>(0)); // ColumnID, RowID
		itemMultiColumnList = new CEGUI::ListboxTextItem(std::to_string(Client::getInstance()->getMyData().numberOfKills));
		m_scoreBoard->setItem(itemMultiColumnList, 1, static_cast<CEGUI::uint>(0)); // ColumnID, RowID
		itemMultiColumnList = new CEGUI::ListboxTextItem(std::to_string(Client::getInstance()->getMyData().numberOfDeaths));
		m_scoreBoard->setItem(itemMultiColumnList, 2, static_cast<CEGUI::uint>(0)); // ColumnID, RowID
		//Add other players
		auto& list = Client::getInstance()->getNetworkPlayersREF().getPlayersREF();

		for (size_t i = 0; i < list.size(); i++)
		{
			m_scoreBoard->addRow();
			CEGUI::ListboxTextItem* itemMultiColumnList;
			itemMultiColumnList = new CEGUI::ListboxTextItem(list.at(i).data.userName);
			itemMultiColumnList->setSelectionBrushImage(CEGUI_TYPE + "Images" + "/GenericBrush");
			m_scoreBoard->setItem(itemMultiColumnList, 0, static_cast<CEGUI::uint>(i + 1)); // ColumnID, RowID

			itemMultiColumnList = new CEGUI::ListboxTextItem(std::to_string(list.at(i).data.numberOfKills));
			m_scoreBoard->setItem(itemMultiColumnList, 1, static_cast<CEGUI::uint>(i + 1)); // ColumnID, RowID,
			itemMultiColumnList = new CEGUI::ListboxTextItem(std::to_string(list.at(i).data.numberOfDeaths));
			m_scoreBoard->setItem(itemMultiColumnList, 2, static_cast<CEGUI::uint>(i + 1)); // ColumnID, RowID
		}
		m_scoreboardExists = true;
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
	return true;
}

bool PlayState::onQuitClick(const CEGUI::EventArgs& e) {
	glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
	return true;	
}
