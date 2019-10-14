#include <Pch/Pch.h>
#include "FindServerState.h"
#include <System/StateManager.h>
#include "MenuState.h"
#include "PlayState.h"

#define GUI_SECTION "FINDSERVERSTATE"

FindServerState::FindServerState()
{
	Client::getInstance()->startup();
	loadGui();
	
	m_serverListRefreshing = true;
	Client::getInstance()->refreshServerList();
}

FindServerState::~FindServerState()
{
	removeAllRows();
	Gui::getInstance()->clearWidgetsInSection(GUI_SECTION);
}

void FindServerState::update(float dt)
{
	if (Input::isKeyPressed(GLFW_KEY_G)) {
		static int i = 0;
		m_serverList->addRow();
		CEGUI::ListboxTextItem* itemMultiColumnList;
		itemMultiColumnList = new CEGUI::ListboxTextItem("Dummy", i );
		itemMultiColumnList->setSelectionBrushImage("TaharezLook/MultiListSelectionBrush");
		m_serverList->setItem(itemMultiColumnList, 0, static_cast<CEGUI::uint>(i)); // ColumnID, RowID
		itemMultiColumnList = new CEGUI::ListboxTextItem(std::string("1337") + "/" + std::string("1337"), i + 1);
		m_serverList->setItem(itemMultiColumnList, 1, static_cast<CEGUI::uint>(i)); // ColumnID, RowID
		i++;
	}
	else if (Input::isKeyPressed(GLFW_KEY_H)) {
		removeAllRows();
	}

	if (m_serverListRefreshing && Client::getInstance()->doneRefreshingServerList())
	{
		loadServersIntoList();
		m_serverListRefreshing = false;
	}
}

void FindServerState::render()
{

}

void FindServerState::loadGui()
{
	m_serverList = static_cast<CEGUI::MultiColumnList*>(Gui::getInstance()->createWidget(GUI_SECTION, "TaharezLook/MultiColumnList", glm::vec4(0.20f, 0.25f, 0.60f, 0.40f), glm::vec4(0.0f), "serverlist"));
	m_serverList->addColumn("Server name", 0, CEGUI::UDim(0.65f, 0));
	m_serverList->addColumn("Players", 1, CEGUI::UDim(0.35f, 0));
	m_serverList->setSelectionMode(CEGUI::MultiColumnList::RowSingle);

	m_backToMenu = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, "TaharezLook/Button", glm::vec4(0.05f, 0.90f, 0.1f, 0.05f), glm::vec4(0.0f), "BackToMenu"));
	m_backToMenu->setText("Go back");
	m_backToMenu->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FindServerState::onBackToMenuClicked, this));

	m_joinServer = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, "TaharezLook/Button", glm::vec4(0.35f, 0.70f, 0.1f, 0.05f), glm::vec4(0.0f), "JoinServer"));
	m_joinServer->setText("Join");
	m_joinServer->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FindServerState::onJoinServerClicked, this));

	m_refreshServerList = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, "TaharezLook/Button", glm::vec4(0.55f, 0.70f, 0.1f, 0.05f), glm::vec4(0.0f), "RefreshServer"));
	m_refreshServerList->setText("Refresh");
	m_refreshServerList->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FindServerState::onRefreshServerListClicked, this));

}

void FindServerState::loadServersIntoList()
{
	auto& servers = Client::getInstance()->getServerList();
	removeAllRows();


	for (size_t i = 0; i < servers.size(); i++) {
		m_serverList->addRow();
		CEGUI::ListboxTextItem* itemMultiColumnList;
		itemMultiColumnList = new CEGUI::ListboxTextItem(servers[i].second.serverName, servers[i].first);
		itemMultiColumnList->setSelectionBrushImage("TaharezLook/MultiListSelectionBrush");
		m_serverList->setItem(itemMultiColumnList, 0, static_cast<CEGUI::uint>(i)); // ColumnID, RowID
		itemMultiColumnList = new CEGUI::ListboxTextItem(std::to_string(servers[i].second.connectedPlayers) + "/" + std::to_string(servers[i].second.maxPlayers), servers[i].first+1);
		m_serverList->setItem(itemMultiColumnList, 1, static_cast<CEGUI::uint>(i)); // ColumnID, RowID
	}
}

void FindServerState::removeAllRows()
{
	for (int i = m_serverList->getRowCount() - 1; i >= 0 ; i--) {
		m_serverList->removeRow(i);
	}
}

bool FindServerState::onBackToMenuClicked(const CEGUI::EventArgs& e)
{
	m_stateManager->clearAllAndSetState(new MenuState());
	return true;
}

bool FindServerState::onJoinServerClicked(const CEGUI::EventArgs& e)
{
	
	CEGUI::ListboxItem* item = m_serverList->getFirstSelectedItem();
	logTrace("ID: {0}", item->getID());
	if (item != NULL)
	{
		std::string serverName = item->getText().c_str();
		unsigned int serverID = item->getID();
		item = m_serverList->getNextSelected(item);
		logTrace("ID: {0}", item->getID());
		if (Client::getInstance()->doesServerExist(serverID))
		{
			const ServerInfo& serverInfo = Client::getInstance()->getServerByID(serverID);
			Client::getInstance()->connectToAnotherServer(serverInfo);
		}


		while (!Client::getInstance()->isConnectedToSever())
		{
			if (Client::getInstance()->connectionFailed()) {
				std::printf("Server is full or in session!\n");
				m_serverList->removeRow(serverID);
				return true;
			}
		}
		glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_stateManager->clearAllAndSetState(new PlayState());
	}

	
	return true;
}

bool FindServerState::onRefreshServerListClicked(const CEGUI::EventArgs& e)
{
	m_serverListRefreshing = true;
	Client::getInstance()->refreshServerList();
	return true;
}
