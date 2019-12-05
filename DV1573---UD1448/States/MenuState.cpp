#include <Pch/Pch.h>
#include "MenuState.h"
#include "LobbyState.h"
#include "FindServerState.h"
#include "PlayState.h"
#include <System/StateManager.h>
#include <System/MemoryUsage.h>


#define GUI_SECTION "MENUSTATE"

MenuState::MenuState()
{
	loadGui();
	MemoryUsage mu;
	mu.printBoth("After menu state init");
}

MenuState::~MenuState()
{
	logTrace("Deleting Menu State..");
	Gui::getInstance()->clearWidgetsInSection(GUI_SECTION);	
}

void MenuState::update(float dt)
{

}

void MenuState::render()
{
	Renderer::getInstance()->renderHUD();
}

void MenuState::loadGui()
{
	/* TITLE */
	HudObject* hudObject = new HudObject("Assets/Textures/title.png", glm::vec2(static_cast<float>(SCREEN_WIDTH/2), static_cast<float>(SCREEN_HEIGHT/2)), glm::vec2(1280, 720));/*new HudObject("Assets/Textures/menu/title2.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(576)), glm::vec2(720, 144));*/
	m_hudHandler.insertHUDObject(hudObject, HUDID::TITLE);
		
	/*m_startWithoutServerBtn = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.05f, 0.55f, 0.1f, 0.05f), glm::vec4(0.0f), "StartWithoutServerBtn"));
	m_startWithoutServerBtn->setText("Start offline");
	m_startWithoutServerBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onStartOfflineClicked, this));*/

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
	SoundHandler::getInstance()->stopSound(ThemeSong0);	
	SoundHandler::getInstance()->freeBuffer(ThemeSong0);
	Renderer::getInstance()->clear();
	glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	m_stateManager->clearAllAndSetState(new PlayState(false));
	return true;
}

bool MenuState::onStartServerClicked(const CEGUI::EventArgs& e)
{
	Renderer::getInstance()->clear();
	m_stateManager->clearAllAndSetState(new LobbyState());
	return true;
}

bool MenuState::onStartClientClicked(const CEGUI::EventArgs& e)
{
	Renderer::getInstance()->clear();
	m_stateManager->clearAllAndSetState(new FindServerState());
	return true;
}

bool MenuState::onExitClicked(const CEGUI::EventArgs& e)
{
	SoundHandler::getInstance()->stopSound(ThemeSong0);
	glfwSetWindowShouldClose(glfwGetCurrentContext(), true);	
	return true;
}
