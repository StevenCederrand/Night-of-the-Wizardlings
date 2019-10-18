#pragma once
#include <Pch/Pch.h>
#include "AttackSpell.h"
#include <Spells/SpellHandler.h>

AttackSpell::AttackSpell(glm::vec3 pos, glm::vec3 direction, const AttackSpellBase* spellBase)
	: Spell(pos, direction)
{
	m_type = NORMALATTACK;
	m_spellBase = spellBase;
	setTravelTime(spellBase->m_lifeTime);

	Transform tempTransform;
	tempTransform.scale = glm::vec3(0.2f, 0.2f, 0.2f);
	setTransform(tempTransform);

	setWorldPosition(pos);
	setDirection(direction);
}

AttackSpell::~AttackSpell()
{
}


void AttackSpell::update(float deltaTime)
{
	translate(getDirection() * deltaTime * m_spellBase->m_speed);
	setTravelTime(getTravelTime() - deltaTime);

	//DEBUG
	//logTrace("Current spell pos {0}, {1}, {2}", getTransform().position.x, getTransform().position.y, getTransform().position.z);
}
