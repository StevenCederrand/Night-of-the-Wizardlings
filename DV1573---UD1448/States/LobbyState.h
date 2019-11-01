#ifndef _LOBBY_STATE_H
#define _LOBBY_STATE_H
#include <Pch/Pch.h>
#include "System/State.h"
#include <Gui/Gui.h>

enum GUIInputID {
	USERNAME, 
	SERVERNAME
};

struct InputBox {
	bool cleared = false; //Indicates if the starting text has been cleared
	std::string defaultText = ""; //Default text of the inputbox  
	std::pair<CEGUI::Editbox*, GUIInputID> inputBox;
};

class LobbyState : public State {

public:
	LobbyState();
	virtual ~LobbyState() override;
	virtual void update(float dt) override;
	virtual void render() override;

private:
	void loadGui();
	void inputHandling();
	void startServer();
	CEGUI::Editbox* getInputBoxByID(const GUIInputID& id) const;

private:
	bool onStartSeverClicked(const CEGUI::EventArgs& e);
	bool onBackToMenuClicked(const CEGUI::EventArgs& e);

private:
	CEGUI::PushButton* m_startServerBtn;
	CEGUI::PushButton* m_backToMenuBtn;
	CEGUI::Combobox* m_serverNameBox;

	std::vector<InputBox> m_inputBoxes;
	int activeText;
};


#endif
