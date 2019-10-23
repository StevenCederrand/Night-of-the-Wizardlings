#pragma once
#include <Pch/Pch.h>
#include "AttackSpell.h"
#include <Spells/SpellHandler.h>

AttackSpell::AttackSpell(glm::vec3 pos, glm::vec3 direction, const AttackSpellBase* spellBase)
	: Spell(pos, direction)
{
	m_type = SPELL_TYPE::NORMALATTACK;
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

const bool& AttackSpell::getHasCollided() const
{
	return m_hasCollided;
}

void AttackSpell::hasCollided()
{
	m_hasCollided = true;
}

void AttackSpell::update(float deltaTime)
{
	setTravelTime(getTravelTime() - deltaTime);

	//DEBUG
}

void AttackSpell::updateRigidbody(float deltaTime, btRigidBody* body)
{
	//shouldAddBounce check if 0.2 second have passed since last bounce add
	if (m_hasCollided && m_shouldAddBounce)
	{	
		m_bounceCounter++;
		m_shouldAddBounce = false;
		
		if (m_bounceCounter == m_spellBase->m_maxBounces + 1)
		{
			logTrace("BOUNCE");
			setTravelTime(0);
		}
	}
	m_bounceTime += deltaTime;
	if (m_bounceTime > 0.2)
	{		
		m_bounceTime = 0;
		m_shouldAddBounce = true;
	}

	setDirection(glm::vec3(body->getLinearVelocity().getX(),
		body->getLinearVelocity().getY(),
		body->getLinearVelocity().getZ()));

	btVector3 rigidBodyPos = body->getWorldTransform().getOrigin();
	setWorldPosition(glm::vec3(rigidBodyPos.getX(), rigidBodyPos.getY(), rigidBodyPos.getZ()));	
}

const AttackSpellBase* AttackSpell::getSpellBase()
{
	return m_spellBase;
}