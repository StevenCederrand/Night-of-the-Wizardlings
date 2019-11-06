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
