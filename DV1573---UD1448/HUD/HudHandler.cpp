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
	HudObject* hudObject = hudObject = new HudObject("Assets/Textures/HUD/CrosshairHP.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(50.0f, 50.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, CROSSHAIR_HP);

	hudObject = new HudObject("Assets/Textures/HUD/CrosshairMANA.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(50.0f, 50.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, CROSSHAIR_MANA); 
	
	hudObject = new HudObject("Assets/Textures/HUD/Crosshair_hit.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(50.0f, 50.0f));
	hudObject->setAlpha(0.0f);
	insertHUDObject(hudObject, CROSSHAIR_HIT);

	hudObject = new HudObject("Assets/Textures/hud/Crosshair.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(50.0f, 50.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, CROSSHAIR);

	hudObject = new HudObject("Assets/Textures/hud/CrosshairDeflect.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(50.0f, 50.0f));
	hudObject->setAlpha(0.0f);
	insertHUDObject(hudObject, CROSSHAIR_DEFLECT);

	hudObject = new HudObject("Assets/Textures/hud/DamageIndicator.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(64.0f, 64.0f));
	hudObject->setAlpha(0.0f);	
	insertHUDObject(hudObject, DAMAGE_INDICATOR);


	// ___ ICONS ___
	hudObject = new HudObject("Assets/Textures/hud/Arcane_BG.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 4) * 3, static_cast<float>(64)), glm::vec2(80.0f, 80.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, SPELL_ARCANE);

	hudObject = new HudObject("Assets/Textures/hud/Shield_BG.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 4) * 3 + 128, static_cast<float>(64)), glm::vec2(80.0f, 80.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, SPELL_DEFLECT);

	hudObject = new HudObject("Assets/Textures/hud/Fire_BG.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 4) * 3 + 64, static_cast<float>(64 + 90)), glm::vec2(80.0f, 80.0f));
	hudObject->setAlpha(1.0f);
	insertHUDObject(hudObject, SPELL_SPECIAL);

	//HP BAR
	hudObject = new HudObject("Assets/Textures/hud/tmpHP.png", glm::vec2(static_cast<float>(0), static_cast<float>(174)), glm::vec2(28.0f, 348.5f));
	hudObject->setAlpha(1.0f);
	hudObject->setFillColor(glm::vec3(1, 0, 0));
	insertHUDObject(hudObject, BAR_HP);

	//Mana bar
	hudObject = new HudObject("Assets/Textures/hud/tmpMana.png", glm::vec2(static_cast<float>(22), static_cast<float>(150)), glm::vec2(17.0f, 300.0f));
	hudObject->setAlpha(1.0f);
	hudObject->setFillColor(glm::vec3(1, 0, 0));
	insertHUDObject(hudObject, BAR_MANA);

	hudObject = new HudObject("Assets/Textures/hud/PickupOverlay.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(static_cast<float>(SCREEN_WIDTH), (static_cast<float>(SCREEN_HEIGHT))));
	hudObject->setAlpha(0.0f);
	insertHUDObject(hudObject, POWERUP);

	hudObject = new HudObject("Assets/Textures/hud/DamageOverlay.png", glm::vec2(static_cast<float>(SCREEN_WIDTH / 2), static_cast<float>(SCREEN_HEIGHT / 2)), glm::vec2(static_cast<float>(SCREEN_WIDTH), (static_cast<float>(SCREEN_HEIGHT))));
	hudObject->setAlpha(0.0f);
	insertHUDObject(hudObject, DAMAGE_OVERLAY);
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

	if (item == m_hudObjects.end()) {
		m_hudObjects[hudID] = object;
	}

	Renderer::getInstance()->submit2DHUD(object);
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
