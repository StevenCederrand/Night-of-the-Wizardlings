#include "Pch/Pch.h"
#include "SettingState.h"
#include "MenuState.h"
#include <System/StateManager.h>
#include <Windows.h>
#include <ShlObj.h>
#include <Shlwapi.h>


#define GUI_SECTION "SETTINGSTATE"

SettingState::SettingState()
{


	int userNumMax = 0;
	m_volymCurrent = GetPrivateProfileInt("DB_SETTINGS", "Volym", 0, "../foobarr.ini");
	m_volymMax = 100.0f;
	
	m_MouseSensCurrent = GetPrivateProfileInt("DB_SETTINGS", "MouseSens", 50, "../foobarr.ini");
	m_MouseSensMax = 100.0f;
	m_FOVBase = 20;

	m_FOVCurrent = GetPrivateProfileInt("DB_SETTINGS", "FoV", 0, "../foobarr.ini");
	m_FOVMax = 100.0f;
	
	loadGui();
}

SettingState::~SettingState()
{
	Gui::getInstance()->clearWidgetsInSection(GUI_SECTION);
}

void SettingState::update(float dt)
{
	
}

void SettingState::render()
{
	Renderer::getInstance()->renderHUD();
}

void SettingState::loadGui()
{

	/* TITLE */
	HudObject* hudObject = new HudObject("Assets/Textures/title.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(1280, 720));/*new HudObject("Assets/Textures/menu/title2.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(576)), glm::vec2(720, 144));*/
	m_hudHandler.insertHUDObject(hudObject, HUDID::TITLE);


	//Sliders
	m_sliderVolym = static_cast<CEGUI::Slider*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Slider", glm::vec4(0.05f, 0.55f, 0.1f, 0.05f), glm::vec4(0.0f), "VolymSlider"));
	m_sliderVolym->setMaxValue(m_volymMax);
	m_sliderVolym->setCurrentValue(m_volymCurrent);
	m_sliderVolym->setClickStep(1.0f);
	m_sliderVolym->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&SettingState::onVolymChange, this));


	m_sliderMouseSens = static_cast<CEGUI::Slider*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Slider", glm::vec4(0.15f, 0.55f, 0.15f, 0.05f), glm::vec4(0.0f), "MouseSensSlider"));
	m_sliderMouseSens->setMaxValue(m_MouseSensMax);
	m_sliderMouseSens->setCurrentValue(m_MouseSensCurrent);
	m_sliderMouseSens->setClickStep(1.0f);
	m_sliderMouseSens->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&SettingState::onMouseSensChange, this));


	m_sliderFOV = static_cast<CEGUI::Slider*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Slider", glm::vec4(0.3f, 0.55f, 0.1f, 0.05f), glm::vec4(0.0f), "FOVSlider"));
	m_sliderFOV->setMaxValue(m_FOVMax);
	m_sliderFOV->setCurrentValue(m_FOVCurrent);
	m_sliderFOV->setClickStep(1.0f);
	m_sliderFOV->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&SettingState::onFOVChange, this));


	//Boxes
	m_editBoxVolym = static_cast<CEGUI::Editbox*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Editbox", glm::vec4(0.05f, 0.50f, 0.1f, 0.05f), glm::vec4(0.0f), "EditBoxVolym"));
	m_editBoxVolym->setText("Volym: " + std::to_string(m_volymCurrent));


	m_editBoxMouseSens = static_cast<CEGUI::Editbox*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Editbox", glm::vec4(0.15f, 0.50f, 0.15f, 0.05f), glm::vec4(0.0f), "EditBoxMouseSens"));
	m_editBoxMouseSens->setText("Mouse Sensitivity: "+ std::to_string(m_MouseSensCurrent));


	m_editBoxFOV = static_cast<CEGUI::Editbox*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Editbox", glm::vec4(0.3f, 0.50f, 0.1f, 0.05f), glm::vec4(0.0f), "EditBoxFov"));
	m_editBoxFOV->setText("FOV: " + std::to_string(m_FOVCurrent));


	//Buttons
	m_SaveBtn = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.05f, 0.65f, 0.1f, 0.05f), glm::vec4(0.0f), "SaveButton"));
	m_SaveBtn->setText("Save");
	m_SaveBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SettingState::OnSaveClicked, this));


	m_BackBth = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.05f, 0.85f, 0.1f, 0.05f), glm::vec4(0.0f), "BackButton"));
	m_BackBth->setText("Back");
	m_BackBth->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SettingState::onBackClicked, this));

}

bool SettingState::onBackClicked(const CEGUI::EventArgs& e)
{
	Renderer::getInstance()->clear();
	m_stateManager->clearAllAndSetState(new MenuState());
	return true;
}

bool SettingState::OnSaveClicked(const CEGUI::EventArgs& e)
{
	
	//bool test = WritePrivateProfileString("DB_SETTINGS", "USER_NUM_MAX", "99", "../foobarr.ini");

	bool volym = WritePrivateProfileString("DB_SETTINGS", "Volym", std::to_string(m_volymCurrent).c_str(), "../foobarr.ini");
	bool mouseSens = WritePrivateProfileString("DB_SETTINGS", "MouseSens", std::to_string(m_MouseSensCurrent).c_str(), "../foobarr.ini");
	bool foV = WritePrivateProfileString("DB_SETTINGS", "FoV", std::to_string(m_FOVCurrent).c_str(), "../foobarr.ini");

	if (!volym)
		logTrace("ERROR_ cant write Volym");
	if (!mouseSens)
		logTrace("ERROR_ cant write MouseSens");
	if (!foV)
		logTrace("ERROR_ cant write FoV");


	int userNumMax = 0;
	userNumMax = GetPrivateProfileInt("DB_SETTINGS", "USER_NUM_MAX", -1, "../foobarr.ini");


	return false;
}


bool SettingState::onVolymChange(const CEGUI::EventArgs& e)
{

	m_volymCurrent = m_sliderVolym->getCurrentValue();
	m_editBoxVolym->setText("Volym: " + std::to_string(m_volymCurrent));
	logTrace("Volym: "+ std::to_string(m_volymCurrent));

	return false;
}

bool SettingState::onMouseSensChange(const CEGUI::EventArgs& e)
{
	m_MouseSensCurrent = m_sliderMouseSens->getCurrentValue();
	m_editBoxMouseSens->setText("Mouse Sensitivity: " + std::to_string(m_MouseSensCurrent));
	logTrace("Mouse Sensitivity: " + std::to_string(m_MouseSensCurrent));

	return false;
}

bool SettingState::onFOVChange(const CEGUI::EventArgs& e)
{
	m_FOVCurrent = m_FOVBase + m_sliderFOV->getCurrentValue();
	m_editBoxFOV->setText("FOV: " + std::to_string(m_FOVCurrent));
	logTrace("FOV: " + std::to_string(m_FOVCurrent));
	return false;
}
