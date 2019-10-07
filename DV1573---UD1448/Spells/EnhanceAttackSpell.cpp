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

void EnhanceAttackSpell::update(float dt)
{
}
