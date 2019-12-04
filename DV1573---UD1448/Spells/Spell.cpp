#include "Pch/Pch.h"
#include "Spell.h"

//Name, Pos, Direction, Speed, TravelTime
Spell::Spell(glm::vec3 pos, glm::vec3 m_direction)
{
	m_type = -1;
	m_travelTime = 0.0f;
	setWorldPosition(pos + m_direction);
	m_bodyRef = nullptr;
}

Spell::~Spell()
{
}

const uint64_t& Spell::getUniqueID() const
{
	return m_uniqueID;
}

const int& Spell::getSoundSlot() const
{
	return m_soundSlot;
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

void Spell::setType(int type)
{
	m_type = type;
}

void Spell::setBodyReference(btRigidBody* body)
{
	m_bodyRef = body;
}

void Spell::setSoundSlot(int slot)
{
	m_soundSlot = slot;
}


const glm::vec3& Spell::getDirection() const {
	return m_direction;
}

void Spell::updateTool(float radius, float speed, float dt)
{
}
