#include <Pch/Pch.h>
#include "PlayState.h"
#include <System/StateManager.h>
#include "MenuState.h"
#include <Networking/Client.h>
#include <Networking/LocalServer.h>

#define PLAYSECTION "PLAYSTATE"

PlayState::PlayState()
{
	m_bPhysics = new BulletPhysics(-20);
	m_spellHandler = new SpellHandler(m_bPhysics);
	m_spellHandler->setOnHitCallback(std::bind(&PlayState::onSpellHit_callback, this));
	
	ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setInt("albedoTexture", 0);
	
	m_camera = new Camera();
	m_player = new Player(m_bPhysics, "Player", glm::vec3(0.0f, 2.0f, 0.0f), m_camera, m_spellHandler);
	Renderer::getInstance()->setupCamera(m_player->getCamera());
	
	//TODO: organized loading system?
	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();

	// HUD
	m_crosshairHUD = new HudObject("Assets/Textures/Crosshair.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(32.0f, 32.0f));
	m_crosshairHUD->setAlpha(1.0f);
	Renderer::getInstance()->submit2DHUD(m_crosshairHUD);
	
	m_hitCrosshair = new HudObject("Assets/Textures/Crosshair_hit.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(32.0f, 32.0f));
	m_hitCrosshair->setAlpha(0.0f);
	Renderer::getInstance()->submit2DHUD(m_hitCrosshair);

	m_deflectCrosshairHUD = new HudObject("Assets/Textures/Crosshair_deflect.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(32.0f, 32.0f));
	m_deflectCrosshairHUD->setAlpha(0.0f);
	Renderer::getInstance()->submit2DHUD(m_deflectCrosshairHUD);

	// ___ ICONS ___
	HudObject* icon = new HudObject("Assets/Textures/hud/Arcane_BG.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 4) * 3, static_cast<float>(64)), glm::vec2(80.0f, 80.0f));
	icon->setAlpha(1.0f);
	Renderer::getInstance()->submit2DHUD(icon);
	m_icons.emplace_back(icon);

	icon = new HudObject("Assets/Textures/hud/Shield_BG.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 4) * 3 + 128, static_cast<float>(64)), glm::vec2(80.0f, 80.0f));
	icon->setAlpha(1.0f);
	Renderer::getInstance()->submit2DHUD(icon);
	m_icons.emplace_back(icon);

	icon = new HudObject("Assets/Textures/hud/Fire_BG.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 4) * 3 + 64, static_cast<float>(64 + 90)), glm::vec2(80.0f, 80.0f));
	icon->setAlpha(1.0f);
	Renderer::getInstance()->submit2DHUD(icon);
	m_icons.emplace_back(icon);
	//HP BAR
	m_hpBar = new HudObject("Assets/Textures/hud/tmpHP.png", glm::vec2(static_cast<float>(80), static_cast<float>(124)), glm::vec2(80.0f, 200.0f));
	m_hpBar->setAlpha(1.0f);
	m_hpBar->setFillColor(glm::vec3(1, 0, 0));
	Renderer::getInstance()->submit2DHUD(m_hpBar);
	// ___ ____ ___

	m_damageOverlay = new HudObject("Assets/Textures/DamageOverlay.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(static_cast<float>(SCREEN_WIDTH), (static_cast<float>(SCREEN_HEIGHT))));
	m_damageOverlay->setAlpha(0.0f);
	Renderer::getInstance()->submit2DHUD(m_damageOverlay);
	m_player->setHealth(NetGlobals::maxPlayerHealth);


	m_objects.push_back(new WorldObject("internalTestmap"));
	m_objects[m_objects.size() - 1]->loadMesh("internalTestmap.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(10.0f, 2.0f, -1.0f));
	Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);
	
	gContactAddedCallback = callbackFunc;
	// Geneterate bullet objects / hitboxes
	for (size_t i = 0; i < m_objects.size(); i++)
	{
		m_objects.at(i)->createRigidBody(CollisionObject::box, m_bPhysics);	
		m_objects.at(i)->createDebugDrawer();
	}

	if(Client::getInstance()->isInitialized())
		Client::getInstance()->assignSpellHandler(m_spellHandler);

}

PlayState::~PlayState()
{

	for (GameObject* object : m_objects)
		delete object;
	for (HudObject* icon : m_icons)
		delete icon;

	GUIclear();

	m_objects.clear();
	delete m_skybox;
	delete m_player;
	delete m_bPhysics;
	delete m_spellHandler;
	delete m_camera;
	delete m_crosshairHUD;
	delete m_deflectCrosshairHUD;
	delete m_hitCrosshair;
	delete m_damageOverlay;
	delete m_hpBar;

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
	
	//m_hpBar->setYClip(m_player->getHealth() / 100);
	auto* clientPtr = Client::getInstance();
	for (PlayerEvents evnt = clientPtr->readNextEvent(); evnt != PlayerEvents::None; evnt = clientPtr->readNextEvent()) {

		switch (evnt) {

			case PlayerEvents::Died: 
			{
				logWarning("[Event system] Died");
				m_camera->disableCameraMovement(true);
				m_crosshairHUD->setAlpha(0.0f);
				m_deflectCrosshairHUD->setAlpha(0.0f);
				break;
			}

			case PlayerEvents::Respawned:
			{
				logWarning("[Event system] Respawned");
				m_player->setPlayerPos(Client::getInstance()->getMyData().latestSpawnPosition);
				m_camera->resetCamera();
				m_camera->disableCameraMovement(false);
				m_crosshairHUD->setAlpha(1.0f);
				m_deflectCrosshairHUD->setAlpha(0.0f);
				break;
			}


			case PlayerEvents::TookDamage:
			{
				logWarning("[Event system] Took damage");
				m_damageOverlay->setAlpha(1.0f);
				m_player->setHealth(Client::getInstance()->getMyData().health);
				break;
			}


			case PlayerEvents::TookHeal:
			{
				logWarning("[Event system] Took a heal");
				break;
			}


			case PlayerEvents::TookPowerup:
			{
				logWarning("[Event system] Took a powerup");
				break;
			}

			case PlayerEvents::PowerupRemoved:
			{
				logWarning("[Event system] Powerup was removed");
				break;
			}

		}


	}
	// Look at the killer when dead ( If he exist )
	if (!m_camera->isCameraActive() && clientPtr->getMyData().health <= 0)
	{
		const PlayerPacket* myKiller = Client::getInstance()->getLatestPlayerThatHitMe();
		if (myKiller != nullptr) {
			m_camera->lookAt(myKiller->position);
		}
	}
	
	// Crosshair switching (Deflect or not)
	if (m_player->isDeflecting()) {
		m_crosshairHUD->setAlpha(0.0f);
		m_deflectCrosshairHUD->setAlpha(1.0f);
	}
	else
	{
		m_crosshairHUD->setAlpha(1.0f);
		m_deflectCrosshairHUD->setAlpha(0.0f);
	}

	// Overlay alpha decrease
	if (m_damageOverlay->getAlpha() > 0.0F)
	{
		m_damageOverlay->setAlpha(m_damageOverlay->getAlpha() - dt);
		if(m_damageOverlay->getAlpha() < 0.0f) {
			m_damageOverlay->setAlpha(0.0f);
		}
	}

	// Hit marker alpha decrease
	if (m_hitCrosshair->getAlpha() > 0.0f) {
		m_hitCrosshair->setAlpha(m_hitCrosshair->getAlpha() - dt);
		
		if (m_hitCrosshair->getAlpha() < 0.0f) {
			m_hitCrosshair->setAlpha(0.0f);
		}
	}

	// Update game objects
	for (GameObject* object : m_objects)
	{
		object->update(dt);
	}

	//Enable GUI
	GUIHandler();

	// Remove this
	if (Input::isKeyReleased(GLFW_KEY_K))
	{
		logTrace("Player position: ({0},{1},{2})", m_player->getPlayerPos().x, m_player->getPlayerPos().y, m_player->getPlayerPos().z);
	}

}

void PlayState::render()
{
	Renderer::getInstance()->render(m_skybox, m_spellHandler);
	//Renderer::getInstance()->renderDebug();
}

void PlayState::onSpellHit_callback()
{
	m_hitCrosshair->setAlpha(1.0f);
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

	if (Client::getInstance()->getServerState().currentState != NetGlobals::GAME_END_STATE && m_endGameBoardVisible) {
		GUIclear();
		m_endGameBoardVisible = false;
	}
	
	if (Input::isKeyPressed(GLFW_KEY_TAB) && !m_endGameBoardVisible) {
		GUILoadScoreboard();
	}
	else if (Client::getInstance()->getServerState().currentState == NetGlobals::GAME_END_STATE && !m_endGameBoardVisible) {
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
		m_scoreBoard = static_cast<CEGUI::MultiColumnList*>(Gui::getInstance()->createWidget(PLAYSECTION, "TaharezLook/MultiColumnList", glm::vec4(0.20f, 0.25f, 0.60f, 0.40f), glm::vec4(0.0f), "Scoreboard"));
		m_scoreBoard->addColumn("PLAYER: ", 0, CEGUI::UDim(0.33f, 0));
		m_scoreBoard->addColumn("KILLS: ", 1, CEGUI::UDim(0.33f, 0));
		m_scoreBoard->addColumn("DEATHS: ", 2, CEGUI::UDim(0.34f, 0));

		//Add the client
		m_scoreBoard->addRow();
		CEGUI::ListboxTextItem* itemMultiColumnList;

		itemMultiColumnList = new CEGUI::ListboxTextItem(Client::getInstance()->getMyData().userName);
		itemMultiColumnList->setSelectionBrushImage("TaharezLook/MultiListSelectionBrush");
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
			itemMultiColumnList->setSelectionBrushImage("TaharezLook/MultiListSelectionBrush");
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
	m_mainMenu = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(PLAYSECTION, "TaharezLook/Button", glm::vec4(0.45f, 0.45f, 0.1f, 0.05f), glm::vec4(0.0f), "Exit To Main Menu"));
	m_mainMenu->setText("Main Menu");
	m_mainMenu->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PlayState::onMainMenuClick, this));

	m_quit = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(PLAYSECTION, "TaharezLook/Button", glm::vec4(0.45f, 0.55f, 0.1f, 0.05f), glm::vec4(0.0f), "QUIT"));
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

//This function is called everytime two collision objects collide
bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1,
	const btCollisionObjectWrapper* obj2, int id2, int index2)
{
	Spell* sp1 = reinterpret_cast<Spell*>(obj1->getCollisionObject()->getUserPointer());
	Spell* sp2 = reinterpret_cast<Spell*>(obj2->getCollisionObject()->getUserPointer());

	 //Currently off, unknown error on reflect and AOE spell // JR
	if (sp1 != nullptr && sp2 == nullptr) {
		logTrace("sp1: Spell collided");
	
		if (!sp1->getHasCollided())
			sp1->hasCollided();	
	}
	
	else if (sp2 != nullptr) {
		
		if (!sp2->getHasCollided())
			sp2->hasCollided();
	}
	return false;
}
