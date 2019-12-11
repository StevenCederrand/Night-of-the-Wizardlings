#include "Pch/Pch.h"
#include "SettingState.h"
#include "MenuState.h"
#include <System/StateManager.h>
#include <Windows.h>
#include "../Renderer/TextRenderer.h"



#define GUI_SECTION "SETTINGSTATE"

SettingState::SettingState()
{


	int userNumMax = 0;
	m_volumeCurrent = GetPrivateProfileInt("DB_SETTINGS", "volume", 0, "Assets/Settings/settings.ini");
	m_volumeMax = 100.0f;
	
	m_MouseSensCurrent = GetPrivateProfileInt("DB_SETTINGS", "MouseSens", 50, "Assets/Settings/settings.ini");
	m_MouseSensMax = 100.0f;

	m_FOVCurrent = GetPrivateProfileInt("DB_SETTINGS", "FoV", 60, "Assets/Settings/settings.ini");
	m_FOVMax = 100.0f;
	m_FOVBase = 20;

	m_boolVSync = GetPrivateProfileInt("DB_SETTINGS", "VSync", 1, "Assets/Settings/settings.ini");
	
	loadGui();
}

SettingState::~SettingState()
{
	Gui::getInstance()->clearWidgetsInSection(GUI_SECTION);
}

void SettingState::update(float dt)
{
	TextManager::getInstance()->update();
}

void SettingState::render()
{
	Renderer::getInstance()->renderHUD();
	TextRenderer::getInstance()->renderText();
}

void SettingState::loadGui()
{

	/* TITLE */
	HudObject* hudObject = new HudObject("Assets/Textures/title.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(1280, 720));/*new HudObject("Assets/Textures/menu/title2.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(576)), glm::vec2(720, 144));*/
	m_hudHandler.insertHUDObject(hudObject, HUDID::TITLE);


	//Sliders
	m_sliderVolume = static_cast<CEGUI::Slider*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Slider", glm::vec4(0.45f, 0.40f, 0.15f, 0.03f), glm::vec4(0.0f), "volumeSlider"));
	m_sliderVolume->setMaxValue(m_volumeMax);
	m_sliderVolume->setCurrentValue(m_volumeCurrent);
	m_sliderVolume->setClickStep(1.0f);
	m_sliderVolume->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&SettingState::onvolumeChange, this));


	m_sliderMouseSens = static_cast<CEGUI::Slider*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Slider", glm::vec4(0.45f, 0.6f, 0.15f, 0.03f), glm::vec4(0.0f), "MouseSensSlider"));
	m_sliderMouseSens->setMaxValue(m_MouseSensMax);
	m_sliderMouseSens->setCurrentValue(m_MouseSensCurrent);
	m_sliderMouseSens->setClickStep(1.0f);
	m_sliderMouseSens->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&SettingState::onMouseSensChange, this));


	m_sliderFOV = static_cast<CEGUI::Slider*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Slider", glm::vec4(0.45f, 0.8f, 0.15f, 0.03f), glm::vec4(0.0f), "FOVSlider"));
	m_sliderFOV->setMaxValue(m_FOVMax);
	m_sliderFOV->setCurrentValue(m_FOVCurrent-m_FOVBase);
	m_sliderFOV->setClickStep(1.0f);
	m_sliderFOV->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&SettingState::onFOVChange, this));


	//Boxes
	//m_editBoxvolume = static_cast<CEGUI::Editbox*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Editbox", glm::vec4(0.45f, 0.35f, 0.1f, 0.03f), glm::vec4(0.0f), "EditBoxvolume"));
	//m_editBoxvolume->setText("volume: " + std::to_string(m_volumeCurrent));


	//m_editBoxMouseSens = static_cast<CEGUI::Editbox*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Editbox", glm::vec4(0.45f, 0.55f, 0.15f, 0.05f), glm::vec4(0.0f), "EditBoxMouseSens"));
	//m_editBoxMouseSens->setText("Mouse Sensitivity: "+ std::to_string(m_MouseSensCurrent));


	//m_editBoxFOV = static_cast<CEGUI::Editbox*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Editbox", glm::vec4(0.45f, 0.75f, 0.1f, 0.05f), glm::vec4(0.0f), "EditBoxFov"));
	//m_editBoxFOV->setText("FOV: " + std::to_string(m_FOVCurrent));


	//Buttons
	m_SaveBtn = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.05f, 0.75f, 0.1f, 0.05f), glm::vec4(0.0f), "SaveButton"));
	m_SaveBtn->setText("Save");
	m_SaveBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SettingState::OnSaveClicked, this));


	m_BackBth = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.05f, 0.85f, 0.1f, 0.05f), glm::vec4(0.0f), "BackButton"));
	m_BackBth->setText("Back");
	m_BackBth->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SettingState::onBackClicked, this));


	m_VSyncBtn = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.475f, 0.95f, 0.1f, 0.05f), glm::vec4(0.0f), "VSyncButton"));
	m_VSyncBtn->setText("VSync");
	m_VSyncBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SettingState::onVSyncClicked, this));


	TextRenderer::getInstance()->init(nullptr);
	m_textVolume = TextManager::getInstance()->addDynamicText("Volume: " + std::to_string(m_volumeCurrent), 0.12f, glm::vec3(0.05f, -0.7f, 0.0f), 1.0f, TextManager::TextBehaviour::StayForever, glm::vec3(0.0f), true);
	m_textSensitivity = TextManager::getInstance()->addDynamicText("Mouse Sensitivity: " + std::to_string(m_MouseSensCurrent), 0.12f, glm::vec3(0.05f, -1.1f, 0.0f), 1.0f, TextManager::TextBehaviour::StayForever, glm::vec3(0.0f), true);
	m_textFOV = TextManager::getInstance()->addDynamicText("FOV: " + std::to_string(m_FOVCurrent), 0.12f, glm::vec3(0.05f, -1.5f, 0.0f), 1.0f, TextManager::TextBehaviour::StayForever, glm::vec3(0.0f), true);
	m_textVSync = TextManager::getInstance()->addDynamicText("VSync: " + std::to_string(m_boolVSync), 0.12f, glm::vec3(0.05f, -1.79f, 0.0f), 1.0f, TextManager::TextBehaviour::StayForever, glm::vec3(0.0f), true);

}

bool SettingState::onBackClicked(const CEGUI::EventArgs& e)
{
	Renderer::getInstance()->clear();
	
	m_stateManager->clearAllAndSetState(new MenuState());
	return true;
}

bool SettingState::OnSaveClicked(const CEGUI::EventArgs& e)
{
	bool volume = WritePrivateProfileString("DB_SETTINGS", "volume", std::to_string(m_volumeCurrent).c_str(), "Assets/Settings/settings.ini");
	bool mouseSens = WritePrivateProfileString("DB_SETTINGS", "MouseSens", std::to_string(m_MouseSensCurrent).c_str(), "Assets/Settings/settings.ini");
	bool foV = WritePrivateProfileString("DB_SETTINGS", "FoV", std::to_string(m_FOVCurrent).c_str(), "Assets/Settings/settings.ini");
	bool VSync =  WritePrivateProfileString("DB_SETTINGS", "VSync", std::to_string(m_boolVSync).c_str(), "Assets/Settings/settings.ini");


	if (!volume)
		logTrace("ERROR_ cant write volume");
	if (!mouseSens)
		logTrace("ERROR_ cant write MouseSens");
	if (!foV)
		logTrace("ERROR_ cant write FoV");

	float tempVolume = m_volumeCurrent * 0.01;
	SoundHandler::getInstance()->setMasterVolume(tempVolume);

	return true;
}

bool SettingState::onVSyncClicked(const CEGUI::EventArgs& e)
{
	m_boolVSync = !m_boolVSync;
	m_textVSync->changeText("VSync: " + std::to_string(m_boolVSync));
	return true;
}


bool SettingState::onvolumeChange(const CEGUI::EventArgs& e)
{

	m_volumeCurrent = m_sliderVolume->getCurrentValue();
	//m_editBoxvolume->setText("volume: " + std::to_string(m_volumeCurrent));
	m_textVolume->changeText("Volume: " + std::to_string(m_volumeCurrent));
	logTrace("volume: "+ std::to_string(m_volumeCurrent));

	return true;
}

bool SettingState::onMouseSensChange(const CEGUI::EventArgs& e)
{
	m_MouseSensCurrent = m_sliderMouseSens->getCurrentValue();
	//m_editBoxMouseSens->setText("Mouse Sensitivity: " + std::to_string(m_MouseSensCurrent));
	m_textSensitivity->changeText("Mouse Sensitivity: " + std::to_string(m_MouseSensCurrent));
	logTrace("Mouse Sensitivity: " + std::to_string(m_MouseSensCurrent));

	return true;
}

bool SettingState::onFOVChange(const CEGUI::EventArgs& e)
{
	m_FOVCurrent = m_FOVBase + m_sliderFOV->getCurrentValue();
	m_textFOV->changeText("FOV: " + std::to_string(m_FOVCurrent));
	//m_editBoxFOV->setText("FOV: " + std::to_string(m_FOVCurrent));
	logTrace("FOV: " + std::to_string(m_FOVCurrent));
	return true;
}
