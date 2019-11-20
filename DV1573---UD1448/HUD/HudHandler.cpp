#include "Pch/Pch.h"
#include "HudHandler.h"

HudHandler::HudHandler()
{

}

HudHandler::~HudHandler()
{
	std::map<HUDID, HudObject*>::iterator it;

	for (it = m_hudObjects.begin(); it != m_hudObjects.end(); it++) {
 		delete it->second;
	}

	m_hudObjects.clear();
}

void HudHandler::loadPlayStateHUD() {
	// HUD
	HudObject* hudObject = new HudObject("Assets/Textures/HUD/CrosshairHP.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(50.0f, 50.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, HUDID::CROSSHAIR_HP);

	hudObject = new HudObject("Assets/Textures/HUD/CrosshairMANA.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(50.0f, 50.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, HUDID::CROSSHAIR_MANA);

	hudObject = new HudObject("Assets/Textures/hud/Crosshair.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(50.0f, 50.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, HUDID::CROSSHAIR);

	hudObject = new HudObject("Assets/Textures/HUD/Crosshair_hit.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(50.0f, 50.0f));
	hudObject->setAlpha(0.0f);
	insertHUDObject(hudObject, HUDID::CROSSHAIR_HIT);

	hudObject = new HudObject("Assets/Textures/hud/CrosshairDeflect.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(50.0f, 50.0f));
	hudObject->setAlpha(0.0f);
	insertHUDObject(hudObject, HUDID::CROSSHAIR_DEFLECT);

	hudObject = new HudObject("Assets/Textures/hud/DamageIndicator.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(64.0f, 64.0f));
	hudObject->setAlpha(0.0f);
	insertHUDObject(hudObject, HUDID::DAMAGE_INDICATOR);

	hudObject = new HudObject("Assets/Textures/hud/deflect_hit.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(70.0f, 70.0f));
	hudObject->setAlpha(0.0f);
	insertHUDObject(hudObject, HUDID::CROSSHAIR_DEFLECT_INDICATOR);

	// ___ ICONS ___
	hudObject = new HudObject("Assets/Textures/hud/Arcane_BG.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 4) * 3, static_cast<float>(64)), glm::vec2(80.0f, 80.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, HUDID::SPELL_ARCANE);

	hudObject = new HudObject("Assets/Textures/hud/Shield_BG.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 4) * 3 + 128, static_cast<float>(64)), glm::vec2(80.0f, 80.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, HUDID::SPELL_DEFLECT);

	hudObject = new HudObject("Assets/Textures/hud/Fire_BG.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 4) * 3 + 64, static_cast<float>(64 + 90)), glm::vec2(80.0f, 80.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, HUDID::SPELL_SPECIAL);

	//HP BAR
	hudObject = new HudObject("Assets/Textures/hud/tmpHP.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(454 / 6)), glm::vec2(static_cast<float>(932 / 3), static_cast<float>(454 / 3)));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, HUDID::BAR_HP);

	//Mana bar
	hudObject = new HudObject("Assets/Textures/hud/tmpMana.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(454 / 6)), glm::vec2(static_cast<float>(932 / 3), static_cast<float>(454 / 3)));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, HUDID::BAR_MANA);

	hudObject = new HudObject("Assets/Textures/hud/PickupOverlay.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(static_cast<float>(SCREEN_WIDTH), (static_cast<float>(SCREEN_HEIGHT))));
	hudObject->setAlpha(0.0f);
	insertHUDObject(hudObject, HUDID::POWERUP);

	hudObject = new HudObject("Assets/Textures/hud/DamageOverlay.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(static_cast<float>(SCREEN_WIDTH), (static_cast<float>(SCREEN_HEIGHT))));
	hudObject->setAlpha(0.0f);
	insertHUDObject(hudObject, HUDID::DAMAGE_OVERLAY);

	hudObject = new HudObject("Assets/Textures/hud/MainFrame.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(454 / 6)), glm::vec2(static_cast<float>(1643 / 3), static_cast<float>(454 / 3)));
	insertHUDObject(hudObject, HUDID::ICON_FRAME);

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
		if (it->first == DAMAGE_OVERLAY || it->first == CROSSHAIR_HIT || it->first == DAMAGE_INDICATOR) {
			continue;
		}
		it->second->setAlpha(1.0f);
	}

}

void HudHandler::insertHUDObject(HudObject* object, const HUDID& hudID)
{
	auto item = m_hudObjects.find(hudID);
	//If the object doesn't exist
	if (item == m_hudObjects.end()) {
 		m_hudObjects[hudID] = object;
	}

	Renderer::getInstance()->submit2DHUD(m_hudObjects[hudID]);
}

HudObject* HudHandler::getHudObject(const HUDID& hudID)
{
	if (hudID != m_cacheID) {
		auto item = m_hudObjects.find(hudID);
		
		if (item == m_hudObjects.end()) {
			return nullptr;
		}
		m_cacheID = hudID;
		m_cache = item._Ptr->_Myval.second;

		return m_cache;
	}
	else {
		return m_cache;
	}

}
