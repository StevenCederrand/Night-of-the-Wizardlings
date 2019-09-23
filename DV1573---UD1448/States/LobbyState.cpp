#include "Pch/Pch.h"
#include "LobbyState.h"
#include <Networking/LocalServer.h>
#include <Networking/Client.h>
#include "MenuState.h"
#include <System/StateManager.h>
#include "PlayState.h"

#define GUI_SECTION "LOBBYSTATE"

LobbyState::LobbyState()
{
	loadGui();
}

LobbyState::~LobbyState()
{
	Gui::getInstance()->clearWidgetsInSection(GUI_SECTION);
}

void LobbyState::update(float dt)
{
}

void LobbyState::render()
{
}

void LobbyState::loadGui()
{

	auto* text = Gui::getInstance()->createWidget(GUI_SECTION, "TaharezLook/StaticText", glm::vec4(0.425f, 0.45f, 0.15f, 0.05f), glm::vec4(0.0f), "LabelText");
	text->setText("Server name:");
	
	m_serverNameBox = static_cast<CEGUI::Combobox*>(Gui::getInstance()->createWidget(GUI_SECTION, "TaharezLook/Combobox", glm::vec4(0.425f, 0.50f, 0.15f, 0.05f), glm::vec4(0.0f), "SeverNameBox"));
	m_serverNameBox->setDragDropTarget(false);

	m_startServerBtn = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, "TaharezLook/Button", glm::vec4(0.425f, 0.55f, 0.15f, 0.05f), glm::vec4(0.0f), "StartServer"));
	m_startServerBtn->setText("Launch server");
	m_startServerBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LobbyState::onStartSeverClicked, this));

	m_backToMenuBtn = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, "TaharezLook/Button", glm::vec4(0.05f, 0.90f, 0.1f, 0.05f), glm::vec4(0.0f), "BackToMenuBtn"));
	m_backToMenuBtn->setText("Go back");
	m_backToMenuBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LobbyState::onBackToMenuClicked, this));

}

bool LobbyState::onStartSeverClicked(const CEGUI::EventArgs& e)
{
	LocalServer::getInstance()->startup(m_serverNameBox->getText().c_str());
	Client::getInstance()->startup();
	Client::getInstance()->connectToMyServer();

	glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	m_stateManager->clearAllAndSetState(new PlayState());
	return true;
}

bool LobbyState::onBackToMenuClicked(const CEGUI::EventArgs& e)
{
	m_stateManager->clearAllAndSetState(new MenuState());
	return true;
}
