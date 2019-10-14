#include "Pch/Pch.h"
#include "Spell.h"

Spell::Spell(glm::vec3 pos)
	: GameObject()
{
	m_spellPosition = pos;
}
//Name, Pos, Direction, Speed, TravelTime
Spell::Spell(std::string name, glm::vec3 pos, glm::vec3 direction, float speed, float travelTime, std::string meshName, float cooldown)
	: GameObject(name)
{
	m_travelTime = travelTime;
	m_spellSpeed = speed;
	m_direction = direction;
	m_spellPosition = pos;
	m_spellCoolDown = cooldown;
	loadMesh(meshName);
	setWorldPosition(pos);
	translate(direction);
}

Spell::~Spell()
{
}

float Spell::getCooldown() const
{
	return m_spellCoolDown;
}

float Spell::getSpellSpeed() const
{
	return m_spellSpeed;
}

float Spell::getTravelTime() const
{
	return m_travelTime;
}

glm::vec3 Spell::getSpellPos() const
{
	return m_spellPosition;
}

glm::vec3 Spell::getDirection() const
{
	return m_direction;
}

void Spell::setDamage(int damage)
{
	m_spellDamage = damage;
}

void Spell::setSpellSpeed(float speed)
{
	m_spellSpeed = speed;
}

void Spell::setSpellPos(glm::vec3 pos)
{
	m_spellPosition = pos;
}

void Spell::setCooldown(float cooldown)
{
	m_spellCoolDown = cooldown;
}

void Spell::setTravelTime(float travelTime)
{
	m_travelTime = travelTime;
}

void Spell::setDirection(glm::vec3 direction)
{
	m_direction = direction;
}

void Spell::updateSpellPos(glm::vec3 newPos)
{
	m_spellPosition += newPos;
}


void Spell::update(float dt)
{
}

void Spell::destroySpell()
{
}
