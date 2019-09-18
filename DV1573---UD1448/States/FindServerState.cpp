#include <Pch/Pch.h>
#include "FindServerState.h"
#include <System/StateManager.h>
#include "MenuState.h"

FindServerState::FindServerState()
{
	//Client::getInstance()->startup();
	m_serverListRefreshing = false;
	m_gui.init();
	logTrace("Find");
	/*m_serverList = static_cast<CEGUI::MultiColumnList*>(m_gui.createWidget("TaharezLook/MultiColumnList", glm::vec4(0.20f, 0.25f, 0.60f, 0.40f), glm::vec4(0.0f), "serverlist"));
	m_serverList->addColumn("Server name", 0, CEGUI::UDim(0.65f, 0));
	m_serverList->addColumn("Players", 1, CEGUI::UDim(0.35f, 0));
	m_serverList->setSelectionMode(CEGUI::MultiColumnList::RowSingle);

	m_backToMenu = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.05f, 0.90f, 0.1f, 0.05f), glm::vec4(0.0f), "BackToMenu"));
	m_backToMenu->setText("Go back");
	m_backToMenu->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FindServerState::onBackToMenuClicked, this));

	m_joinServer = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.55f, 0.1f, 0.05f), glm::vec4(0.0f), "StartServer"));
	m_joinServer->setText("Join");
	m_joinServer->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FindServerState::onJoinServerClicked, this));

	m_refreshServerList = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.65f, 0.1f, 0.05f), glm::vec4(0.0f), "JoinServer"));
	m_refreshServerList->setText("Refresh");
	m_refreshServerList->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FindServerState::onRefreshServerListClicked, this));*/
}

FindServerState::~FindServerState()
{
	m_gui.destroy();
}

void FindServerState::update(float dt)
{
	if (Input::isKeyHeldDown(GLFW_KEY_F3)) {
		m_stateManager->clearAllAndSetState(new MenuState());
	}

	m_gui.update(dt);

	if (m_serverListRefreshing && Client::getInstance()->doneRefreshingServerList())
	{
		// Todo: fill column list

		m_serverListRefreshing = false;
	}
}

void FindServerState::render()
{
	m_gui.draw();
}

bool FindServerState::onBackToMenuClicked(const CEGUI::EventArgs& e)
{
	m_stateManager->clearAllAndSetState(new MenuState());
	return true;
}

bool FindServerState::onJoinServerClicked(const CEGUI::EventArgs& e)
{
	// Todo: Get the server from the columm list and join
	return true;
}

bool FindServerState::onRefreshServerListClicked(const CEGUI::EventArgs& e)
{
	// Todo: Clear the column list
	m_serverListRefreshing = true;
	Client::getInstance()->refreshServerList();
	return true;
}
