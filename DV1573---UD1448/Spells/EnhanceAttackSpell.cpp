#include "Pch/Pch.h"
#include "EnhanceAttackSpell.h"

EnhanceAttackSpell::EnhanceAttackSpell(glm::vec3 pos)
	: Spell(pos)
{
}

EnhanceAttackSpell::EnhanceAttackSpell(std::string name, glm::vec3 pos, glm::vec3 direction, float speed, float travelTime, std::string meshName, float cooldown, float nrOfEnhancedAttacks, float attackCooldown, float spellActiveTime)
	: Spell(name, pos, direction, speed, travelTime, meshName, cooldown)
{
	this->nrOfAttacks = nrOfEnhancedAttacks;
	this->spellActiveTime = spellActiveTime;
}

EnhanceAttackSpell::~EnhanceAttackSpell()
{
}

float EnhanceAttackSpell::getNrOfAttacks()
{
	return this->nrOfAttacks;
}

float EnhanceAttackSpell::getAttackCooldown()
{
	return this->attackCooldown;
}

void EnhanceAttackSpell::setNrOfAttacks(float nrOfEnhancedAttacks)
{
	 this->nrOfAttacks = nrOfEnhancedAttacks;
}

void EnhanceAttackSpell::reduceNrOfAttacks(float nrOfEnhancedAttacks)
{
	this->nrOfAttacks -= nrOfEnhancedAttacks;
}

void EnhanceAttackSpell::setAttackCooldown(float attackCooldown)
{
	this->attackCooldown = attackCooldown;
}

void EnhanceAttackSpell::updateActiveSpell(float deltaTime)
{
	translate(getDirection() * deltaTime * getSpellSpeed());
	setTravelTime(getTravelTime() - 1 * deltaTime);
}

void EnhanceAttackSpell::spellCooldownUpdate(float deltaTime)
{
	if (getCooldown() > 0)
		setCooldown(getCooldown() - 1 * deltaTime);
}

void EnhanceAttackSpell::attackCooldownUpdate(float deltaTime)
{
	if (getAttackCooldown() > 0)
	{
		setAttackCooldown(getAttackCooldown() - 1 * deltaTime);
	}
}

void EnhanceAttackSpell::createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector)
{
	setSpellPos(glm::vec3(spellPos.x, spellPos.y - 1.8f, spellPos.z) + directionVector); //-1.8 = spwn point for spell, spell need to be 0 and playerPos is set to (0,1.8,0)
	translate(getSpellPos());
	setDirection(directionVector);
}

void EnhanceAttackSpell::update(float dt)
{
}
