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
	if (m_serverListRefreshing && Client::getInstance()->doneRefreshingServerList())
	{
		loadServersIntoList();
		m_serverListRefreshing = false;
	}
	//We want to clear the usernamebox upon selection
	if (m_inputTextOpen && m_usernameBox->isActive() && !m_inputTextSelected) {
		//Upon the selection frame
		m_inputTextSelected = true;
		if (m_inputTextSelected) {
			m_usernameBox->setText(""); //Clear the text box upon selection
		}
	}
}

void FindServerState::render()
{

}

void FindServerState::loadGui()
{
	//Serverlist window
	m_serverList = static_cast<CEGUI::MultiColumnList*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/MultiColumnList", glm::vec4(0.20f, 0.25f, 0.60f, 0.40f), glm::vec4(0.0f), "serverlist"));
	m_serverList->addColumn("Server name", 0, CEGUI::UDim(0.65f, 0));
	m_serverList->addColumn("Players", 1, CEGUI::UDim(0.30f, 0));
	m_serverList->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
	m_serverList->setShowHorzScrollbar(false);
	
	//Main menu
	m_backToMenu = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.05f, 0.90f, 0.1f, 0.05f), glm::vec4(0.0f), "BackToMenu"));
	m_backToMenu->setText("Go back");
	m_backToMenu->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FindServerState::onBackToMenuClicked, this));

	//Join server button
	m_joinServer = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.35f, 0.70f, 0.1f, 0.05f), glm::vec4(0.0f), "JoinServer"));
	m_joinServer->setText("Join");
	m_joinServer->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FindServerState::onJoinServerClicked, this));

	//Refresh server list button
	m_refreshServerList = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.55f, 0.70f, 0.1f, 0.05f), glm::vec4(0.0f), "RefreshServer"));
	m_refreshServerList->setText("Refresh");
	m_refreshServerList->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FindServerState::onRefreshServerListClicked, this));

	//The button to close the username input window
	m_backToList = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.55f, 0.70f, 0.1f, 0.05f), glm::vec4(0.0f), "Close-username-input"));
	m_backToList->setText("Return");
	m_backToList->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FindServerState::onBackToListClicked, this));
	m_backToList->hide();

	//Username Input
	m_usernameBox = static_cast<CEGUI::Editbox*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Editbox", glm::vec4(0.425f, 0.55f, 0.15f, 0.05f), glm::vec4(0.0f), "username(join)"));
	m_usernameBox->setMaxTextLength(16);
	m_usernameBox->setText("Enter Username...");
	m_usernameBox->hide();	
}

void FindServerState::loadServersIntoList()
{
	auto& servers = Client::getInstance()->getServerList();
	removeAllRows();


	for (size_t i = 0; i < servers.size(); i++) {
		m_serverList->addRow();
		CEGUI::ListboxTextItem* itemMultiColumnList;
		itemMultiColumnList = new CEGUI::ListboxTextItem(servers[i].second.serverName, servers[i].first);
		itemMultiColumnList->setSelectionBrushImage(CEGUI_TYPE + "Images" + "/GenericBrush");
		itemMultiColumnList->setSelectionColours(CEGUI::Colour(0.8f, 0.8f, 0.0f));
		m_serverList->setItem(itemMultiColumnList, 0, static_cast<CEGUI::uint>(i)); // ColumnID, RowID
		itemMultiColumnList = new CEGUI::ListboxTextItem(std::to_string(servers[i].second.connectedPlayers) + "/" + std::to_string(servers[i].second.maxPlayers), servers[i].first+1);
		m_serverList->setItem(itemMultiColumnList, 1, static_cast<CEGUI::uint>(i)); // ColumnID, RowID
		itemMultiColumnList->setSelectionColours(CEGUI::Colour(0.8f, 0.8f, 0.0f));
	}
}

void FindServerState::removeAllRows()
{
	for (int i = m_serverList->getRowCount() - 1; i >= 0 ; i--) {
		m_serverList->removeRow(i);
	}
}

void FindServerState::usernameInput()
{
	//is write text open?
	if (!m_inputTextOpen) {
		m_serverList->hide();
		m_refreshServerList->hide();

		m_usernameBox->show();
		m_backToList->show();

		m_inputTextOpen = true;
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
	
	
	if (item != NULL)
	{
		usernameInput();
		if (m_usernameBox->getText() == "Enter Username...") {
			return false;
		}
		std::string serverName = item->getText().c_str();
		unsigned int serverID = item->getID();
		item = m_serverList->getNextSelected(item);
	
		if (Client::getInstance()->doesServerExist(serverID))
		{
			const ServerInfo& serverInfo = Client::getInstance()->getServerByID(serverID);
			Client::getInstance()->connectToAnotherServer(serverInfo);
			Client::getInstance()->setUsername(m_usernameBox->getText().c_str());
		}


		while (!Client::getInstance()->isConnectedToSever())
		{
			if (Client::getInstance()->connectionFailed()) {
				std::printf("Server is full or in session!\n");
				m_serverList->removeRow(serverID);
				return true;
			}
		}
		SoundHandler::getInstance()->stopSound(ThemeSong0);
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

bool FindServerState::onBackToListClicked(const CEGUI::EventArgs& e)
{
	//Hide the button and the username input
	m_usernameBox->hide();
	m_backToList->hide();
	//Open up the server list
	m_serverList->show();
	m_refreshServerList->show();
	m_inputTextOpen = false;

	m_usernameBox->setText("Enter Username...");
	m_inputTextSelected = false;
	return true;
}
