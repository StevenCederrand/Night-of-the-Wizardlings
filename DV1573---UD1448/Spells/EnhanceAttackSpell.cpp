#include "Pch/Pch.h"
#include "EnhanceAttackSpell.h"
#include <Spells/SpellHandler.h>


EnhanceAttackSpell::EnhanceAttackSpell(const EnhanceHanderSpellBase* spellBase)
	: Spell(glm::vec3(0), glm::vec3(0))
{
	m_type = ENHANCEATTACK;
	m_spellBase = spellBase;

	setWorldPosition(glm::vec3(0));
	setDirection(glm::vec3(0));
}

EnhanceAttackSpell::~EnhanceAttackSpell()
{
}

float EnhanceAttackSpell::getNrOfAttacks() const
{
	return m_nrOfAttacks;
}

float EnhanceAttackSpell::getAttackCooldown() const
{
	return m_attackCooldown;
}

void EnhanceAttackSpell::setNrOfAttacks(float nrOfEnhancedAttacks)
{
	 m_nrOfAttacks = nrOfEnhancedAttacks;
}

void EnhanceAttackSpell::reduceNrOfAttacks(float nrOfEnhancedAttacks)
{
	m_nrOfAttacks -= nrOfEnhancedAttacks;
}

void EnhanceAttackSpell::setAttackCooldown(float attackCooldown)
{
	m_attackCooldown = attackCooldown;
}

void EnhanceAttackSpell::attackCooldownUpdate(float deltaTime)
{
	if (getAttackCooldown() > 0)
	{
		setAttackCooldown(getAttackCooldown() - 1 * deltaTime);
	}
}


//void EnhanceAttackSpell::update(float deltaTime)
//{
//	if (m_attackCooldown)
//	{
//		EnhanceAttackSpell tempSpell2 = *m_tempEnhanceAttackSpell;
//		tempSpell2.createSpell(deltaTime, spellPos, directionVector);
//		m_enhanceAttackSpell.push_back(tempSpell2);
//		m_tempEnhanceAttackSpell->setAttackCooldown(0.3f);
//		m_tempEnhanceAttackSpell->reduceNrOfAttacks(1.0f);
//	}
//	if (m_tempEnhanceAttackSpell->getNrOfAttacks() <= 0)
//	{
//		m_tempEnhanceAttackSpell->setCooldown(10.0f);
//		m_tempEnhanceAttackSpell->setNrOfAttacks(3);
//
//		//-----Return true if the spell is done in order to get the normal attack back-----//
//		m_tempSpell->setCooldown(1.0f);
//		setType(NORMALATTACK);
//		spellIsOver = true;
//	}
//
//
//
//
//
//	translate(getDirection() * deltaTime * m_spellBase->m_speed);
//	setTravelTime(getTravelTime() - 1 * deltaTime);
//}
