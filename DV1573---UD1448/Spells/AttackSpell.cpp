#pragma once
#include <Pch/Pch.h>
#include "AttackSpell.h"

AttackSpell::AttackSpell() 
	: Spell(glm::vec3(0))
{
}

AttackSpell::AttackSpell(glm::vec3 pos)
	: Spell(pos)
{
}

AttackSpell::AttackSpell(std::string name, glm::vec3 pos, glm::vec3 direction, float speed, float travelTime, std::string meshName, float cooldown)
	: Spell(name, pos, direction, speed, travelTime, meshName, cooldown)
{
}

AttackSpell::~AttackSpell()
{
}

void AttackSpell::updateActiveSpell(float deltaTime)
{
	
	translate(getDirection() * deltaTime * getSpellSpeed());
	setTravelTime(getTravelTime() - 1 * deltaTime);

}

void AttackSpell::createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector)
{
	setSpellPos(glm::vec3(spellPos.x, spellPos.y - 1.8f, spellPos.z) + directionVector); //-1.8 = spwn point for spell, spell need to be 0 and playerPos is set to (0,1.8,0)
	translate(getSpellPos());
	setDirection(directionVector);
}

void AttackSpell::spellCooldownUpdate(float deltaTime)
{
	if (getCooldown() > 0)
		setCooldown(getCooldown() - 1 * deltaTime);
}

void AttackSpell::update(float dt)
{
}


