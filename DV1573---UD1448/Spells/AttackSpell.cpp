 #pragma once
#include <Pch/Pch.h>
#include "AttackSpell.h"
#include <Spells/SpellHandler.h>

AttackSpell::AttackSpell(glm::vec3 pos, glm::vec3 direction, const SpellBase* spellBase)
	: Spell(pos, direction, spellBase)
{
	m_type = OBJECT_TYPE::NORMALATTACK;
	m_spellBase = spellBase;
	setTravelTime(spellBase->m_lifeTime);

	Transform tempTransform;
	tempTransform.position = pos;
	tempTransform.scale = glm::vec3(spellBase->m_radius, spellBase->m_radius, spellBase->m_radius);
	
	setTransform(tempTransform);
	setDirection(direction);
}

AttackSpell::AttackSpell(glm::vec3 pos, OBJECT_TYPE type) 
	: Spell(pos, glm::vec3(0), nullptr)
{
	m_type = type;

	Transform tempTransform;
	tempTransform.scale = glm::vec3(0.2f, 0.2f, 0.2f); // Hardcoded for networkd. Needs to change.
	tempTransform.position = pos;

	setTransform(tempTransform);
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

	btRigidBody* body = getRigidBody();
	//shouldAddBounce check if 0.2 second have passed since last bounce add
	if (m_hasCollided && m_shouldAddBounce)
	{
		m_bounceCounter++;
		m_shouldAddBounce = false;
		m_hasCollided = false;

		if (m_bounceCounter > m_spellBase->m_maxBounces)
		{
			setTravelTime(0);
			return;
		}
	}
	m_bounceTime += deltaTime;
	if (m_bounceTime > 0.2)
	{
		m_bounceTime = 0;
		m_shouldAddBounce = true;
	}

	body->applyCentralImpulse(
		body->getLinearVelocity().normalized() * (1.0f +
			m_spellBase->m_acceleration * body->getMass() * deltaTime));
	
	glm::vec3 forceDirection = glm::vec3(body->getLinearVelocity().getX(), body->getLinearVelocity().getY(), body->getLinearVelocity().getZ());
	setDirection(forceDirection);
	
	setTransform(getRigidTransform());
}

const float AttackSpell::getDamage()
{
	return (Randomizer::single(m_spellBase->m_lowDamage, m_spellBase->m_highDamage));
}

const glm::vec3& AttackSpell::getPos() const
{
	return glm::vec3(0);
}