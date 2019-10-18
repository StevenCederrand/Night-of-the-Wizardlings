#include "Pch/Pch.h"
#include "Spell.h"

//Name, Pos, Direction, Speed, TravelTime
Spell::Spell(glm::vec3 pos, glm::vec3 m_direction)
{
	m_type = -1;
	setWorldPosition(pos + m_direction);

	
}

Spell::~Spell()
{
}

const uint64_t& Spell::getUniqueID() const
{
	return m_uniqueID;
}

void Spell::setTravelTime(float travelTime)
{
	m_travelTime = travelTime;
}

void Spell::setUniqueID(const uint64_t& uniqueID)
{
	m_uniqueID = uniqueID;
}

void Spell::setDirection(glm::vec3 direction)
{
	m_direction = direction;
}

