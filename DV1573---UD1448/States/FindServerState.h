#ifndef _FIND_SERVER_STATE_STATE_H
#define _FIND_SERVER_STATE_STATE_H
#include <Pch/Pch.h>
#include "System/State.h"
#include <Gui/Gui.h>
#include <Networking/Client.h>


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

private:
	CEGUI::PushButton* m_backToMenu;
	CEGUI::PushButton* m_joinServer;
	CEGUI::PushButton* m_refreshServerList;
	CEGUI::MultiColumnList* m_serverList;
	CEGUI::Combobox* m_usernameBox;

	bool m_serverListRefreshing;
	bool m_inputTextOpen;

private:
	bool onBackToMenuClicked(const CEGUI::EventArgs& e);
	bool onJoinServerClicked(const CEGUI::EventArgs& e);
	bool onRefreshServerListClicked(const CEGUI::EventArgs& e);
};


#endif
