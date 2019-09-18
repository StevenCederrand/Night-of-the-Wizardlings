#include <Pch/Pch.h>
#include "MenuState.h"
#include "FindServerState.h"
#include "PlayState.h"
#include <System/StateManager.h>

MenuState::MenuState()
{
	//bool guiFirstTimeInit = true;
	//if (Gui::getRenderer() != nullptr) 
	//	guiFirstTimeInit = false;
	//
	m_gui.init();
	logTrace("Menu");
	//if (guiFirstTimeInit) {
	//	logTrace("Loading fonts and scheme");
	//	m_gui.loadScheme("TaharezLook.scheme");
	//	m_gui.setFont("DejaVuSans-10");
	//}

	//m_startWithoutServerBtn = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.45f, 0.1f, 0.05f), glm::vec4(0.0f), "StartWithoutServerBtn"));
	//m_startWithoutServerBtn->setText("Start offline");
	//m_startWithoutServerBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onStartOfflineClicked, this));

	//m_startServerBtn = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.55f, 0.1f, 0.05f), glm::vec4(0.0f), "StartServerButton"));
	//m_startServerBtn->setText("Start Server");
	//m_startServerBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onStartServerClicked, this));

	//m_joinServerBtn = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.65f, 0.1f, 0.05f), glm::vec4(0.0f), "JoinServerButton"));
	//m_joinServerBtn->setText("Join Server");
	//m_joinServerBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onStartClientClicked, this));

	//m_quitBtn = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.75f, 0.1f, 0.05f), glm::vec4(0.0f), "ExitButton"));
	//m_quitBtn->setText("Exit");
	//m_quitBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onExitClicked, this));

}

MenuState::~MenuState()
{
	m_gui.destroy();
}

void MenuState::update(float dt)
{
	if (Input::isKeyHeldDown(GLFW_KEY_F3)) {
		m_stateManager->clearAllAndSetState(new FindServerState());
	}
	m_gui.update(dt);
}

void MenuState::render()
{
	m_gui.draw();
}

bool MenuState::onStartOfflineClicked(const CEGUI::EventArgs& e)
{
	glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	m_stateManager->clearAllAndSetState(new PlayState());
	return true;
}

bool MenuState::onStartServerClicked(const CEGUI::EventArgs& e)
{
	logInfo("Whould be nice to have a server");
	return true;
}

bool MenuState::onStartClientClicked(const CEGUI::EventArgs& e)
{
	m_stateManager->clearAllAndSetState(new FindServerState());
	return true;
}

bool MenuState::onExitClicked(const CEGUI::EventArgs& e)
{
	glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
	return true;
}
