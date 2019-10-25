#include "Pch/Pch.h"
#include "EnhanceAttackSpell.h"
#include <Spells/SpellHandler.h>


EnhanceAttackSpell::EnhanceAttackSpell()
{
	m_currentAttackCooldown = m_attackCooldown;
	m_currentAttack = m_nrOfAttacks;


	m_done = true;
	m_ready = false;
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
	m_currentAttack = nrOfEnhancedAttacks;
}

void EnhanceAttackSpell::reduceNrOfAttacks(float nrOfEnhancedAttacks)
{
	m_currentAttack -= nrOfEnhancedAttacks;
}

void EnhanceAttackSpell::setAttackCooldown(float attackCooldown)
{
	m_currentAttackCooldown = attackCooldown;
}

void EnhanceAttackSpell::update(float deltaTime)
{
	if (!m_done)
	{
		m_currentAttackCooldown -= deltaTime;

		if (m_currentAttackCooldown <= 0)
		{
			m_ready = true;

			m_currentAttackCooldown = m_attackCooldown;
			m_currentAttack -= 1;
		}
		if (m_currentAttack <= 0)
		{
			m_currentAttack = m_nrOfAttacks;
			m_done = true;
		}
	}
}

void EnhanceAttackSpell::start()
{
	m_done = false;
}

void EnhanceAttackSpell::attacked()
{
	m_ready = false;
}
