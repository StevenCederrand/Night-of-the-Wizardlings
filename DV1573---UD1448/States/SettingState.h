#ifndef _SETTINGSTATE_H
#define _SETTINGSTATE_H
#include <Pch/Pch.h>
#include <System/State.h>
#include <Gui/Gui.h>
#include <HUD/HudHandler.h>
#include "../BetterText/GUIText.h"


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
	bool onVSyncClicked(const CEGUI::EventArgs& e);
	bool onvolumeChange(const CEGUI::EventArgs& e);
	bool onMouseSensChange(const CEGUI::EventArgs& e);
	bool onFOVChange(const CEGUI::EventArgs& e);



	
	CEGUI::PushButton* m_BackBth;
	CEGUI::PushButton* m_SaveBtn;
	CEGUI::PushButton* m_VSyncBtn;

	CEGUI::Slider* m_sliderVolume;
	CEGUI::Slider* m_sliderMouseSens;
	CEGUI::Slider* m_sliderFOV;


	CEGUI::Editbox* m_editBoxvolume;
	CEGUI::Editbox* m_editBoxMouseSens;
	CEGUI::Editbox* m_editBoxFOV;

	GUIText* m_textVolume;
	GUIText* m_textSensitivity;
	GUIText* m_textFOV;
	GUIText* m_textVSync;

	HudHandler m_hudHandler; //Basic HUD objects


	int m_volumeCurrent;
	int m_MouseSensCurrent;
	int m_FOVCurrent;
	int m_FOVBase;

	float m_volumeMax;
	float m_MouseSensMax;
	float m_FOVMax;

	bool m_boolVSync;



};


#endif // !SETTINGSTATE_H
