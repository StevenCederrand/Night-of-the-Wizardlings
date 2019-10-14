#include "Pch/Pch.h"
#include "EnhanceAttackSpell.h"



EnhanceAttackSpell::EnhanceAttackSpell(glm::vec3 pos)
	: Spell(pos)
{
}

EnhanceAttackSpell::EnhanceAttackSpell(std::string name, glm::vec3 pos, glm::vec3 direction, float speed, float travelTime, std::string meshName, float cooldown, float nrOfEnhancedAttacks)
	: Spell(name, pos, direction, speed, travelTime, meshName, cooldown)
{
	this->nrOfEnhancedAttacks = nrOfEnhancedAttacks;
}

EnhanceAttackSpell::~EnhanceAttackSpell()
{
}

float EnhanceAttackSpell::getThreeAttacks()
{
	return this->nrOfEnhancedAttacks;
}

void EnhanceAttackSpell::setThreeAttacks(float nrOfEnhancedAttacks)
{
	 this->nrOfEnhancedAttacks = nrOfEnhancedAttacks;
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

void EnhanceAttackSpell::createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector)
{
	setSpellPos(glm::vec3(spellPos.x, spellPos.y - 1.8f, spellPos.z) + directionVector); //-1.8 = spwn point for spell, spell need to be 0 and playerPos is set to (0,1.8,0)
	translate(getSpellPos());
	setDirection(directionVector);
}

void EnhanceAttackSpell::update(float dt)
{
}
