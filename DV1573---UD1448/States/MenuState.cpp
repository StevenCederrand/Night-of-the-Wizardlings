#include <Pch/Pch.h>
#include "MenuState.h"
#include "LobbyState.h"
#include "FindServerState.h"
#include "PlayState.h"
#include <System/StateManager.h>

#define GUI_SECTION "MENUSTATE"

MenuState::MenuState()
{
	loadGui();
}

MenuState::~MenuState()
{
	logTrace("Deleting Menu State..");
	Gui::getInstance()->clearWidgetsInSection(GUI_SECTION);
}

void MenuState::update(float dt)
{
	if (Input::isKeyHeldDown(GLFW_KEY_F3)) {
		m_stateManager->clearAllAndSetState(new FindServerState());
	}
	if (Input::isKeyHeldDown(GLFW_KEY_X)) {
		m_rotVal += 0.01f;
		m_hudHandler.getHudObject(TITLE)->setRotation(glm::quat(glm::vec3(0, 0, m_rotVal)));
	}
	
	if (Input::isKeyHeldDown(GLFW_KEY_Z)) {
		m_rotVal -= 0.01f;
		m_hudHandler.getHudObject(TITLE)->setRotation(glm::quat(glm::vec3(0, 0, m_rotVal)));
	}
}

void MenuState::render()
{
	Renderer::getInstance()->renderHUD();
}

void MenuState::loadGui()
{
	/* TITLE */
	/*HudObject* hudObject = hudObject = new HudObject("Assets/Textures/menu/background.png", glm::vec2(0, 0), glm::vec2(SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2));
	m_hudHandler.insertHUDObject(hudObject, HUDID::BACKGROUND);*/
	HudObject*  hudObject = new HudObject("Assets/Textures/menu/title2.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(576)), glm::vec2(720, 144));
	hudObject->setRotation(glm::quat(glm::vec3(0, 0, 0)));
	m_hudHandler.insertHUDObject(hudObject, HUDID::TITLE);


	m_startWithoutServerBtn = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.05f, 0.55f, 0.1f, 0.05f), glm::vec4(0.0f), "StartWithoutServerBtn"));
	m_startWithoutServerBtn->setText("Start offline");
	m_startWithoutServerBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onStartOfflineClicked, this));

	m_startServerBtn = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.05f, 0.65f, 0.1f, 0.05f), glm::vec4(0.0f), "StartServerButton"));
	m_startServerBtn->setText("Start Server");
	m_startServerBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onStartServerClicked, this));

	m_joinServerBtn = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.05f, 0.75f, 0.1f, 0.05f), glm::vec4(0.0f), "JoinServerButton"));
	m_joinServerBtn->setText("Join Server");
	m_joinServerBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onStartClientClicked, this));

	m_quitBtn = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.05f, 0.85f, 0.1f, 0.05f), glm::vec4(0.0f), "ExitButton"));
	m_quitBtn->setText("Exit");
	m_quitBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onExitClicked, this));

}

bool MenuState::onStartOfflineClicked(const CEGUI::EventArgs& e)
{
	glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	Renderer::getInstance()->clear();
	m_stateManager->clearAllAndSetState(new PlayState(false));
	return true;
}

bool MenuState::onStartServerClicked(const CEGUI::EventArgs& e)
{
	m_stateManager->clearAllAndSetState(new LobbyState());
	Renderer::getInstance()->clear();
	return true;
}

bool MenuState::onStartClientClicked(const CEGUI::EventArgs& e)
{
	m_stateManager->clearAllAndSetState(new FindServerState());
	Renderer::getInstance()->clear();
	return true;
}

bool MenuState::onExitClicked(const CEGUI::EventArgs& e)
{
	glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
	return true;
}
