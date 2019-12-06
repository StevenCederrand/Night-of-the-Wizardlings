#ifndef _SETTINGSTATE_H
#define _SETTINGSTATE_H
#include <Pch/Pch.h>
#include <System/State.h>
#include <Gui/Gui.h>
#include <HUD/HudHandler.h>

class SettingState: public State 
{
public:
	SettingState();
	~SettingState();

	virtual void update(float dt);
	virtual void render();

private:
	void loadGui();
	
	bool onBackClicked(const CEGUI::EventArgs& e);
	bool OnSaveClicked(const CEGUI::EventArgs& e);
	bool onVolymChange(const CEGUI::EventArgs& e);



	
	CEGUI::PushButton* m_BackBth;
	CEGUI::PushButton* m_SaveBtn;
	CEGUI::Slider* m_slider;
	CEGUI::Editbox* m_editBox;

	HudHandler m_hudHandler; //Basic HUD objects

	int m_volymCurrent;
	float m_volymMax;
};


#endif // !SETTINGSTATE_H
