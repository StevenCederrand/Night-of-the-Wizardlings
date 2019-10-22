#pragma once
#include <Pch/Pch.h>
#include <Spells/SpellHandler.h>
#include <GameObject/GameObject.h>
#include <Spells/Spell.h>

class EnhanceAttackSpell
{
public:
	EnhanceAttackSpell();
	~EnhanceAttackSpell();

	float getNrOfAttacks() const;
	float getAttackCooldown() const;
	float getCooldown() const { return m_coolDown; }

	void setNrOfAttacks(float nrOfEnhancedAttacks);
	void reduceNrOfAttacks(float nrOfEnhancedAttacks);
	void setAttackCooldown(float attackCooldown);
	void update(float deltaTime);

	void start();
	bool isComplete() { return m_done; }
	bool canAttack() { return m_ready; }
	void attacked();

private:

	const float m_coolDown = 5.0f;
	const float m_attackCooldown = 0.2f;
	const float m_nrOfAttacks = 3;
	float m_currentAttackCooldown;
	float m_currentAttack;


	bool m_ready;
	bool m_done;
};