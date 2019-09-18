#ifndef _MENUSTATE_H
#define _MENUSTATE_H
#include <Pch/Pch.h>
#include <System/State.h>
#include <Gui/Gui.h>


typedef CEGUI::PushButton* Cbtn;

class MenuState : public State {

public:
	MenuState();
	virtual ~MenuState() override;
	virtual void update(float dt) override;
	virtual void render() override;

private:
	Gui m_gui;
	Cbtn m_quitBtn, m_joinServerBtn, m_startServerBtn, m_startWithoutServerBtn;

private:
	bool onStartOfflineClicked(const CEGUI::EventArgs& e);
	bool onStartServerClicked(const CEGUI::EventArgs& e);
	bool onStartClientClicked(const CEGUI::EventArgs& e);
	bool onExitClicked(const CEGUI::EventArgs& e);

};


#endif
