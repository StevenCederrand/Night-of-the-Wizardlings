#include "Pch/Pch.h"
#include "Spell.h"

//Name, Pos, Direction, Speed, TravelTime
Spell::Spell(glm::vec3 pos, glm::vec3 m_direction)
{
	//m_travelTime = m_travelTime;
	//m_spellSpeed = speed;
	//m_direction = m_direction;
	//m_position = pos;
	//m_spellCoolDown = cooldown;
	m_type = -1;
	setWorldPosition(pos + m_direction);
}

Spell::~Spell()
{
}

void Spell::setTravelTime(float travelTime)
{
	m_travelTime = travelTime;
}

void Spell::setDirection(glm::vec3 direction)
{
	m_direction = direction;
}
