#include <Pch/Pch.h>
#include "GameGUI.h"

#define GUI_PLAY "PlayState"

GameGUI::GameGUI()
{
}

GameGUI::~GameGUI()
{
}

void GameGUI::loadGUI()
{
	m_mainMenu = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_PLAY, "TaharezLook/Button", glm::vec4(0.45f, 0.45f, 0.1f, 0.05f), glm::vec4(0.0f), "StartWithoutServerBtn"));
	m_mainMenu->setText("Start offline");
	m_mainMenu->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onStartOfflineClicked, this));
}

void GameGUI::unloadGUI()
{
}
