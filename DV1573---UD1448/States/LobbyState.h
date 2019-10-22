#ifndef _LOBBY_STATE_H
#define _LOBBY_STATE_H
#include <Pch/Pch.h>
#include "System/State.h"
#include <Gui/Gui.h>

class LobbyState : public State {

public:
	LobbyState();
	virtual ~LobbyState() override;
	virtual void update(float dt) override;
	virtual void render() override;

private:
	void loadGui();
private:
	bool onStartSeverClicked(const CEGUI::EventArgs& e);
	bool onBackToMenuClicked(const CEGUI::EventArgs& e);

private:
	CEGUI::PushButton* m_startServerBtn;
	CEGUI::PushButton* m_backToMenuBtn;
	CEGUI::Combobox* m_serverNameBox;
	CEGUI::Editbox* m_sNameInput;
	CEGUI::Editbox* m_uNameInput;
};


#endif
