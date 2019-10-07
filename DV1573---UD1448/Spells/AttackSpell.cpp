#pragma once
#include <Pch/Pch.h>
#include "AttackSpell.h"

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

void AttackSpell::update(float dt)
{
}


