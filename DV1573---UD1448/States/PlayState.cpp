#include <Pch/Pch.h>
#include "PlayState.h"
#include <System/StateManager.h>
#include "MenuState.h"
#include <Networking/Client.h>
#include <Networking/LocalServer.h>

#define PLAYSECTION "PLAYSTATE"

PlayState::PlayState()
{
	m_bPhysics = new BulletPhysics(-10);
	m_spellHandler = new SpellHandler(m_bPhysics);
	ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setInt("albedoTexture", 0);
	Renderer::getInstance();
	m_camera = new Camera();
	m_player = new Player(m_bPhysics, "Player", glm::vec3(0.0f, 10.8f, 0.0f), m_camera, m_spellHandler);

	Renderer::getInstance()->setupCamera(m_player->getCamera());
	//TODO: organized loading system?
	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();

	//Test enviroment with 4 meshes inside 1 GameObject, inherited transforms
	m_objects.push_back(new WorldObject("TestScene"));
	m_objects[m_objects.size() - 1]->loadMesh("TestScene.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);
	
	//
	//////Cube and sphere centered in scene
	//m_objects.push_back(new WorldObject("TestCube"));
	//m_objects[m_objects.size() - 1]->loadMesh("TestCube.mesh");
	//m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(5.0f, 0.0f, 0.0f));
	//Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);

	//m_objects.push_back(new WorldObject("TestSphere"));
	//m_objects[m_objects.size() - 1]->loadMesh("TestSphere.mesh");
	//m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(10.0f, 2.0f, -4.0f));
	////m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(5.0f, 1.0f, -2.0f));
	//Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);

	////m_objects.push_back(new WorldObject("TestCube"));
	////m_objects[m_objects.size() - 1]->loadMesh("Playground.mesh");
	////m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(10.0f, 2.0f, -1.0f));
	////Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);
	//
	////////Animated rectangle
	//m_objects.push_back(new AnimatedObject("TestRectangle"));
	//m_objects[m_objects.size() - 1]->loadMesh("TestRectangle.mesh");
	//m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(0.0f, 0.0f, -4.0f));
	//Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], ANIMATEDSTATIC);
	//
	////Animated goblino
	//m_objects.push_back(new AnimatedObject("TestGoblino"));
	//m_objects[m_objects.size() - 1]->loadMesh("ElGoblino.mesh");
	//Transform tempTransform;
	//tempTransform.scale = glm::vec3(0.03f, 0.03f, 0.03f);
	//tempTransform.position = glm::vec3(-3.0f, 0.0f, 3.0f);
	//m_objects[m_objects.size() - 1]->setTransform(tempTransform);
	//Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], ANIMATEDSTATIC);

	
	gContactAddedCallback = callbackFunc;
	// Geneterate bullet objects / hitboxes
	for (int i = 0; i < m_objects.size(); i++)
	{
		
		m_objects.at(i)->createRigidBody(CollisionObject::box, m_bPhysics);	
		m_objects.at(i)->createDebugDrawer();
	}
	logTrace("Playstate created");
}

PlayState::~PlayState()
{
	logTrace("Deleting playstate..");
	for (GameObject* object : m_objects)
		delete object;

	m_objects.clear();
	delete m_skybox;
	delete m_player;
	delete m_bPhysics;
	delete m_spellHandler;
	delete m_camera;
	GUIclear();
	
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
	Renderer::getInstance()->update(dt);
	m_spellHandler->spellUpdate(dt);
	m_player->update(dt);

	for (GameObject* object : m_objects)
	{
		object->update(dt);
	}

	//Enable GUI
	GUIHandler();

	if (Input::isKeyPressed(GLFW_KEY_P)) {
		auto& list = Client::getInstance()->getNetworkSpells();
		logTrace("Active spells on client: {0}", list.size());
	}
}

void PlayState::render()
{
	//Move the render skybox to be a private renderer function
	Renderer::getInstance()->renderSkybox(*m_skybox);
	Renderer::getInstance()->render();
	m_spellHandler->renderSpell();
	Renderer::getInstance()->renderDebug();	
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

	if (Input::isKeyPressed(GLFW_KEY_TAB)) {
		//Create the scoreboard
		m_scoreBoard = static_cast<CEGUI::MultiColumnList*>(Gui::getInstance()->createWidget(PLAYSECTION, "TaharezLook/MultiColumnList", glm::vec4(0.20f, 0.25f, 0.60f, 0.40f), glm::vec4(0.0f), "Scoreboard"));
		m_scoreBoard->addColumn("Player: ", 0, CEGUI::UDim(0.33f, 0));
		m_scoreBoard->addColumn("Score: ", 1, CEGUI::UDim(0.33f, 0));
		m_scoreBoard->addColumn("Deaths: ", 2, CEGUI::UDim(0.34f, 0));
		
		//Add the client
		m_scoreBoard->addRow();
		CEGUI::ListboxTextItem* itemMultiColumnList;

		itemMultiColumnList = new CEGUI::ListboxTextItem(Client::getInstance()->getMyData().userName);
		itemMultiColumnList->setSelectionBrushImage("TaharezLook/MultiListSelectionBrush");
		m_scoreBoard->setItem(itemMultiColumnList, 0, static_cast<CEGUI::uint>(0)); // ColumnID, RowID

		//Add other players
		auto& list = Client::getInstance()->getNetworkPlayersREF().getPlayersREF();

		for (size_t i = 0; i < list.size(); i++)
		{
			m_scoreBoard->addRow();
			CEGUI::ListboxTextItem* itemMultiColumnList;
			itemMultiColumnList = new CEGUI::ListboxTextItem(list.at(i).data.userName);
			//itemMultiColumnList = new CEGUI::ListboxTextItem("Player " + std::to_string(i + 1));
			itemMultiColumnList->setSelectionBrushImage("TaharezLook/MultiListSelectionBrush");
			m_scoreBoard->setItem(itemMultiColumnList, 0, static_cast<CEGUI::uint>(i + 1)); // ColumnID, RowID
		}		 
	}
	if (Input::isKeyReleased(GLFW_KEY_TAB)) {
		GUIclear();
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

	if (sp1 != nullptr && sp2 == nullptr) {
		logTrace("sp1: Spell collided");

		int local = sp1->getLocalBounce();
		int bounce = sp1->getNrofBounce();
		if (local == bounce){
			glm::vec3 normal = glm::vec3(cp.m_normalWorldOnB.getX(), cp.m_normalWorldOnB.getY(), cp.m_normalWorldOnB.getZ());
			sp2->setBounceNormal(normal);
		}
	}
	//if sp2 is a spell get the normal of the manifoldpoint and send that to the spell
	else if (sp2 != nullptr) {
		int local = sp2->getLocalBounce();
		int bounce = sp2->getNrofBounce();
		if (local == bounce){
		glm::vec3 normal = glm::vec3(cp.m_normalWorldOnB.getX(), cp.m_normalWorldOnB.getY(), cp.m_normalWorldOnB.getZ());
		sp2->setBounceNormal(normal);
		}
	}


	return false;
}
