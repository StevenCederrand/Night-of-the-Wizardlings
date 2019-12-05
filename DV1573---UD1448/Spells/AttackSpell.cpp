 #pragma once
#include <Pch/Pch.h>
#include "AttackSpell.h"
#include <Spells/SpellHandler.h>

AttackSpell::AttackSpell(glm::vec3 pos, glm::vec3 direction, AttackSpellBase* spellBase)
	: Spell(pos, direction)
{
	m_type = OBJECT_TYPE::NORMALATTACK;
	m_spellBase = spellBase;
	setTravelTime(spellBase->m_lifeTime);

	Transform tempTransform;

	tempTransform.scale = glm::vec3(spellBase->m_radius, spellBase->m_radius, spellBase->m_radius);
	setTransform(tempTransform);
	setWorldPosition(pos);
	setDirection(direction);
}

AttackSpell::AttackSpell(glm::vec3 pos, OBJECT_TYPE type) : Spell(pos, glm::vec3(0))
{
	m_type = type;
	m_spellBase = nullptr;

    mySpellLoader.LoadProjectileSpell("bestSpell.spell");

	Transform tempTransform;
	tempTransform.scale = glm::vec3(mySpellLoader.m_projectile.m_radius, mySpellLoader.m_projectile.m_radius, mySpellLoader.m_projectile.m_radius);
	setTransform(tempTransform);
	setWorldPosition(pos);
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
    //updateScale += 1 * deltaTime;
    //tempTransformTest.scale = glm::vec3(m_spellBase->m_radius * updateScale, m_spellBase->m_radius * updateScale, m_spellBase->m_radius * updateScale);
   // setTransform(tempTransformTest);
	setTravelTime(getTravelTime() - deltaTime);
}

void AttackSpell::updateRigidbody(float deltaTime, btRigidBody* body)
{
	//shouldAddBounce check if 0.2 second have passed since last bounce add
	if (m_hasCollided && m_shouldAddBounce)
	{	
		m_bounceCounter++;
		m_shouldAddBounce = false;
		m_hasCollided = false;
		
		if (m_bounceCounter > m_spellBase->m_maxBounces)
		{
			//logTrace("BOUNCE");
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

	setDirection(glm::vec3(body->getLinearVelocity().getX(),
		body->getLinearVelocity().getY(),
		body->getLinearVelocity().getZ()));

	btVector3 rigidBodyPos = body->getWorldTransform().getOrigin();
	setWorldPosition(glm::vec3(rigidBodyPos.getX(), rigidBodyPos.getY(), rigidBodyPos.getZ()));	
}

const float AttackSpell::getDamage()
{
	return (Randomizer::single(m_spellBase->m_lowDamage, m_spellBase->m_highDamage));
}

const glm::vec3& AttackSpell::getPos() const
{
	return glm::vec3(0);
}

void AttackSpell::updateTool(float radius, float speed, float dt)
{
    updateScale = radius;
    updateSpeed += speed * dt;
    tempTransformTest.rotation = glm::vec3(0,0,0);
    if (tempTransformTest.position.x > 10)
    {
        tempTransformTest.position = glm::vec3(-2, 3, -10);
        updateSpeed = -2;
    }
    else
        tempTransformTest.position = glm::vec3(updateSpeed,3,-10);

    tempTransformTest.scale = glm::vec3(m_spellBase->m_radius + updateScale, m_spellBase->m_radius + updateScale, m_spellBase->m_radius + updateScale);
    setTransform(tempTransformTest);
}
