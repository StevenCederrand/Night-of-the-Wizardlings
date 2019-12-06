#include "Pch/Pch.h"
#include "SettingState.h"
#include <System/StateManager.h>


#define GUI_SECTION "SETTINGSTATE"

SettingState::SettingState()
{
	m_volymCurrent = 0.0f;
	m_volymMax = 100.0f;

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


	m_slider = static_cast<CEGUI::Slider*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Slider", glm::vec4(0.05f, 0.55f, 0.1f, 0.05f), glm::vec4(0.0f), "VolymSlider"));
	m_slider->setMaxValue(m_volymMax);
	m_slider->setClickStep(1.0f);
	m_slider->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&SettingState::onVolymChange, this));


	m_editBox = static_cast<CEGUI::Editbox*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Editbox", glm::vec4(0.05f, 0.50f, 0.1f, 0.05f), glm::vec4(0.0f), "EditBox"));


	m_SaveBtn = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.05f, 0.65f, 0.1f, 0.05f), glm::vec4(0.0f), "SaveButton"));
	m_SaveBtn->setText("Save");
	m_SaveBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SettingState::OnSaveClicked, this));


	m_BackBth = static_cast<CEGUI::PushButton*>(Gui::getInstance()->createWidget(GUI_SECTION, CEGUI_TYPE + "/Button", glm::vec4(0.05f, 0.85f, 0.1f, 0.05f), glm::vec4(0.0f), "BackButton"));
	m_BackBth->setText("Back");
	m_BackBth->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SettingState::onBackClicked, this));

}

bool SettingState::onBackClicked(const CEGUI::EventArgs& e)
{
	return false;
}

bool SettingState::OnSaveClicked(const CEGUI::EventArgs& e)
{
	return false;
}


bool SettingState::onVolymChange(const CEGUI::EventArgs& e)
{

	m_volymCurrent = m_slider->getCurrentValue();
	m_editBox->setText(std::to_string(m_volymCurrent));
	logTrace("Volym: "+ std::to_string(m_volymCurrent));

	return false;
}
