#include "Pch/Pch.h"
#include "Spell.h"

Spell::Spell(glm::vec3 pos)
	: GameObject()
{
	this->spellPosition = pos;
}
//Name, Pos, Direction, Speed, TravelTime
Spell::Spell(std::string name, glm::vec3 pos, glm::vec3 direction, float speed, float travelTime, std::string meshName, float cooldown)
	: GameObject(name)
{
	this->travelTime = travelTime;
	this->spellSpeed = speed;
	this->direction = direction;
	this->spellPosition = pos;
	this->spellCoolDown = cooldown;
	loadMesh(meshName);
	setWorldPosition(pos);
	translate(direction);
}

Spell::~Spell()
{
}

void Spell::CreateSpellObject()
{
}

void Spell::updateSpell()
{

}

float Spell::getCooldown()
{
	return this->spellCoolDown;
}

float Spell::getSpellSpeed()
{
	return this->spellSpeed;
}

float Spell::getTravelTime()
{
	return this->travelTime;
}

glm::vec3 Spell::getSpellPos()
{
	return spellPosition;
}

glm::vec3 Spell::getDirection()
{
	return this->direction;
}

void Spell::setDamage(int damage)
{
	this->spellDamage = damage;
}

void Spell::setSpellSpeed(float speed)
{
	this->spellSpeed = speed;
}

void Spell::setSpellPos(glm::vec3 pos)
{
	this->spellPosition = pos;
}

void Spell::setCooldown(float cooldown)
{
	this->spellCoolDown = cooldown;
}

void Spell::setTravelTime(float travelTime)
{
	this->travelTime = travelTime;
}

void Spell::setDirection(glm::vec3 direction)
{
	this->direction = direction;
}


void Spell::update(float dt)
{
}

void Spell::destroySpell()
{
}
