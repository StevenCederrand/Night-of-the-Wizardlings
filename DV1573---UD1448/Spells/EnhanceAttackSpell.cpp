#include "Pch/Pch.h"
#include "EnhanceAttackSpell.h"

EnhanceAttackSpell::EnhanceAttackSpell(glm::vec3 pos)
	: Spell(pos)
{
}

EnhanceAttackSpell::EnhanceAttackSpell(std::string name, glm::vec3 pos, glm::vec3 direction, float speed, float travelTime, std::string meshName, float cooldown, float nrOfEnhancedAttacks, float attackCooldown)
	: Spell(name, pos, direction, speed, travelTime, meshName, cooldown)
{
	this->nrOfAttacks = nrOfEnhancedAttacks;
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

void EnhanceAttackSpell::update(float dt)
{
}
