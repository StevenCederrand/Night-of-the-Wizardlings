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

const int& AttackSpell::getNrofBounce() const
{
	return m_nrOfBounce;
}

const int& AttackSpell::getLocalBounce() const
{
	return m_localBounce;
}

void AttackSpell::setBounceNormal(glm::vec3& normal)
{
	m_bounceNormal = normal;
	m_localBounce++;
}

void AttackSpell::update(float deltaTime)
{
	setTravelTime(getTravelTime() - deltaTime);

	//DEBUG
}

void AttackSpell::updateRigidbody(float deltaTime, btRigidBody* body)
{
	//setNewDir check if 0.2 second have passed since last calculation
	//and check of localbounce (callback func) is the same as spells own bounce
	//and calculate the reflect
	if (m_localBounce != m_nrOfBounce && m_setNewDir == true)
	{		
		//m_bounceNormal = glm::vec3(std::ceil(m_bounceNormal.x * 1000.0) / 1000.0, 
		//	std::ceil(m_bounceNormal.y * 1000.0) / 1000.0,
		//	std::ceil(m_bounceNormal.z * 1000.0) / 1000.0);	//Round to two decimals

		//m_bounceNormal = glm::normalize(m_bounceNormal);

		//if (m_bounceNormal.x < 0.001 && m_bounceNormal.x > -0.001)
		//{
		//	m_bounceNormal.x = 0.0f;
		//}
		//if (m_bounceNormal.y < 0.001 && m_bounceNormal.y > -0.001)
		//{
		//	m_bounceNormal.y = 0.0f;
		//}
		//if (m_bounceNormal.z < 0.001 && m_bounceNormal.z > -0.001)
		//{
		//	m_bounceNormal.z = 0.0f;
		//}
		////reflect
		//glm::vec3 oldDir = getDirection();
		//float cosOfAngle = (glm::dot(m_bounceNormal, -oldDir));

		///*if (cosOfAngle < 0.0f)
		//{
		//	m_bounceNormal = -m_bounceNormal;
		//}*/
		//
		//glm::vec3 nlength = (glm::dot(m_bounceNormal, -oldDir) * m_bounceNormal);
		//glm::vec3 u = nlength + oldDir;
		//glm::vec3 newDir = -oldDir + 2 * u;
		//newDir = glm::normalize(newDir);

		//setDirection(newDir);		

	
		m_bounceCounter++;
		m_setNewDir = false;

		
		if (m_bounceCounter == m_spellBase->m_maxBounces + 1)
		{
			setTravelTime(0);
		}
	}
	m_bounceTime += deltaTime;
	if (m_bounceTime > 0.2)
	{		
		m_bounceTime = 0;
		m_nrOfBounce = m_localBounce;
		m_setNewDir = true;
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