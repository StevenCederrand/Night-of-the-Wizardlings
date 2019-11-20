#ifndef _MENUSTATE_H
#define _MENUSTATE_H
#include <Pch/Pch.h>
#include <System/State.h>
#include <Gui/Gui.h>
#include <HUD/HudHandler.h>

class MenuState : public State {

public:
	MenuState();
	virtual ~MenuState() override;
	virtual void update(float dt) override;
	virtual void render() override;
private:
	void loadGui();

private:
	float startY;
	float m_rotVal;
	CEGUI::PushButton* m_quitBtn; 
	CEGUI::PushButton* m_joinServerBtn; 
	CEGUI::PushButton* m_startServerBtn; 
	CEGUI::PushButton* m_startWithoutServerBtn;
	HudHandler m_hudHandler; //Basic HUD objects
private:
	bool onStartOfflineClicked(const CEGUI::EventArgs& e);
	bool onStartServerClicked(const CEGUI::EventArgs& e);
	bool onStartClientClicked(const CEGUI::EventArgs& e);
	bool onExitClicked(const CEGUI::EventArgs& e);
};


#endif
