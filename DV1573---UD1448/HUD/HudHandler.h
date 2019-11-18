#include <Pch/Pch.h>
#include <HUD/HudObject.h>
#include <Renderer/Renderer.h>

#ifndef _HUDHANDLER_h
#define _HUDHANDLER_h

enum HUDID {
	NORMAL, 
	BAR_HP, 
	BAR_MANA, 
	CROSSHAIR,
	CROSSHAIR_HP,
	CROSSHAIR_HP_FRAME, 	//HP AND MANA AROUND THE CROSSHAIR
	CROSSHAIR_MANA, //HP AND MANA AROUND THE CROSSHAIR
	CROSSHAIR_MANA_FRAME,
	CROSSHAIR_HIT, 
	CROSSHAIR_DEFLECT,
	DAMAGE_OVERLAY,
	DAMAGE_INDICATOR,
	SPELL_ARCANE, 
	SPELL_SPECIAL,
	SPELL_DEFLECT, 
	POWERUP,
	TITLE, 
	BACKGROUND
};

class HudHandler {
public:
	HudHandler();
	~HudHandler();

	void loadPlayStateHUD();
	void insertHUDObject(HudObject* object, const HUDID& hudID);
	void fadeOut();
	void fadeIn();

	HudObject* getHudObject(const HUDID& hudID);

private: 
	std::map<HUDID, HudObject*> m_hudObjects;
	HUDID m_cacheID;
	HudObject* m_cache;
};

#endif