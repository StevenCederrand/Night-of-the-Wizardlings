#include "Pch/Pch.h"
#include "HudHandler.h"

HudHandler::HudHandler()
{
	// HUD
	HudObject* hudObject = new HudObject("Assets/Textures/Crosshair_hit.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(32.0f, 32.0f));
	hudObject->setAlpha(0.0f);
	insertHUDObject(hudObject, CROSSHAIR_HIT);

	hudObject = new HudObject("Assets/Textures/Crosshair.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(32.0f, 32.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, CROSSHAIR);

	hudObject = new HudObject("Assets/Textures/Crosshair_deflect.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(32.0f, 32.0f));
	hudObject->setAlpha(0.0f);
	insertHUDObject(hudObject, CROSSHAIR_DEFLECT);


	// ___ ICONS ___
	hudObject = new HudObject("Assets/Textures/hud/Arcane_BG.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 4) * 3, static_cast<float>(64)), glm::vec2(80.0f, 80.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, SPELL_ARCANE);

	hudObject = new HudObject("Assets/Textures/hud/Shield_BG.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 4) * 3 + 128, static_cast<float>(64)), glm::vec2(80.0f, 80.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, SPELL_DEFLECT);

	hudObject = new HudObject("Assets/Textures/hud/Fire_BG.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 4) * 3 + 64, static_cast<float>(64 + 90)), glm::vec2(80.0f, 80.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, SPELL_FIRE);

	//HP BAR
	hudObject = new HudObject("Assets/Textures/hud/tmpHP.png", glm::vec2(static_cast<float>(80), static_cast<float>(124)), glm::vec2(80.0f, 200.0f));
	hudObject->setAlpha(1.0f);
	hudObject->setFillColor(glm::vec3(1, 0, 0));
	insertHUDObject(hudObject, BAR_HP);
	// ___ ____ ___

	hudObject = new HudObject("Assets/Textures/DamageOverlay.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(static_cast<float>(SCREEN_WIDTH), (static_cast<float>(SCREEN_HEIGHT))));
	hudObject->setAlpha(0.0f);
	insertHUDObject(hudObject, DAMAGE_OVERLAY);
}

HudHandler::~HudHandler()
{
	std::map<HUDID, HudObject*>::iterator it;

	for (it = m_hudObjects.begin(); it != m_hudObjects.end(); it++) {
		delete it->second;
	}

	m_hudObjects.clear();
}

void HudHandler::fadeOut() {
	std::map<HUDID, HudObject*>::iterator it;

	for (it = m_hudObjects.begin(); it != m_hudObjects.end(); it++) {
		it->second->setAlpha(0);
	}

}

void HudHandler::fadeIn() {
	std::map<HUDID, HudObject*>::iterator it;

	for (it = m_hudObjects.begin(); it != m_hudObjects.end(); it++) {
		if (it->first == DAMAGE_OVERLAY || it->first == CROSSHAIR_HIT) {
			continue;
		}
		it->second->setAlpha(1.0f);
	}

}

void HudHandler::insertHUDObject(HudObject* object, const HUDID& hudID)
{
	auto item = m_hudObjects.find(hudID);

	if (item == m_hudObjects.end()) {
		m_hudObjects[hudID] = object;
	}

	Renderer::getInstance()->submit2DHUD(object);
}

HudObject* HudHandler::getHudObject(const HUDID& hudID)
{
	auto item = m_hudObjects.find(hudID);

	if (item == m_hudObjects.end()) {
		return nullptr;
	}
	return item._Ptr->_Myval.second;
}
