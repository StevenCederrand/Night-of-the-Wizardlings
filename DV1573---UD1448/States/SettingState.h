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
	bool onMouseSensChange(const CEGUI::EventArgs& e);
	bool onFOVChange(const CEGUI::EventArgs& e);



	
	CEGUI::PushButton* m_BackBth;
	CEGUI::PushButton* m_SaveBtn;

	CEGUI::Slider* m_sliderVolym;
	CEGUI::Slider* m_sliderMouseSens;
	CEGUI::Slider* m_sliderFOV;


	CEGUI::Editbox* m_editBoxVolym;
	CEGUI::Editbox* m_editBoxMouseSens;
	CEGUI::Editbox* m_editBoxFOV;

	HudHandler m_hudHandler; //Basic HUD objects

	int m_volymCurrent;
	int m_MouseSensCurrent;
	int m_FOVCurrent;
	int m_FOVBase;

	float m_volymMax;
	float m_MouseSensMax;
	float m_FOVMax;
};


#endif // !SETTINGSTATE_H
