#pragma once
#include <Pch/Pch.h>
#include <Spells/SpellHandler.h>
#include <GameObject/GameObject.h>
#include <Spells/Spell.h>

class EnhanceAttackSpell : public Spell
{
public:
	EnhanceAttackSpell(const EnhanceHanderSpellBase* spellBase);
	~EnhanceAttackSpell();

	float getNrOfAttacks() const;
	float getAttackCooldown() const;

	void setNrOfAttacks(float nrOfEnhancedAttacks);
	void reduceNrOfAttacks(float nrOfEnhancedAttacks);
	void setAttackCooldown(float attackCooldown);
	void attackCooldownUpdate(float deltaTime);

	void update(float deltaTime);

private:
	const EnhanceHanderSpellBase* m_spellBase;


	float m_attackCooldown;
	float m_nrOfAttacks;

};