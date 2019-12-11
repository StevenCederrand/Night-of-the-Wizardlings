#ifndef _FIND_SERVER_STATE_STATE_H
#define _FIND_SERVER_STATE_STATE_H
#include <Pch/Pch.h>
#include "System/State.h"
#include <Gui/Gui.h>
#include <Networking/Client.h>
#include <HUD/HudHandler.h>


class FindServerState : public State {

public:
	FindServerState();
	virtual ~FindServerState() override;
	virtual void update(float dt) override;
	virtual void render() override;

private:
	void loadGui();
	void loadServersIntoList();
	void removeAllRows();
	void usernameInput();
	void ipInput();

private:
	CEGUI::PushButton* m_backToMenu;
	CEGUI::PushButton* m_backToList;
	CEGUI::PushButton* m_joinServer;
	CEGUI::PushButton* m_spectateServer;
	CEGUI::PushButton* m_refreshServerList;
	CEGUI::PushButton* m_connectViaIP;
	CEGUI::MultiColumnList* m_serverList;
	CEGUI::Editbox* m_usernameBox;
	CEGUI::Editbox* m_ipInput;
	bool m_serverListRefreshing;
	bool m_inputTextOpen;
	bool m_ipTextOpen;
	bool m_ipTextSelected;
	bool m_inputTextSelected;
	HudHandler m_hudHandler; //Basic HUD objects

private:
	bool onBackToMenuClicked(const CEGUI::EventArgs& e);
	bool onBackToListClicked(const CEGUI::EventArgs& e);
	bool onJoinServerClicked(const CEGUI::EventArgs& e);
	bool onConnectViaIPClicked(const CEGUI::EventArgs& e);
	bool onRefreshServerListClicked(const CEGUI::EventArgs& e);
	bool onSpectateServerClicked(const CEGUI::EventArgs& e);
};


#endif
