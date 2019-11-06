#include <Pch/Pch.h>
#include <HUD/HudObject.h>
#include <Renderer/Renderer.h>

#ifndef _HUDHANDLER_h
#define _HUDHANDLER_h

enum HUDID {
	SPECIAL_SPELL,
	DEFLECT, 
	NORMAL, 
	BAR_HP, 
	BAR_MANA, 
	CROSSHAIR,
	CROSSHAIR_HIT, 
	CROSSHAIR_DEFLECT, 
	DAMAGE_OVERLAY,
	SPELL_ARCANE, 
	SPELL_DEFLECT, 
	SPELL_FIRE 
};

class HudHandler {
public:
	HudHandler();
	~HudHandler();

	void insertHUDObject(HudObject* object, const HUDID& hudID);
	void fadeOut();
	void fadeIn();
	HudObject* getHudObject(const HUDID& hudID);

private: 
	std::map<HUDID, HudObject*> m_hudObjects;
	   
};

#endif