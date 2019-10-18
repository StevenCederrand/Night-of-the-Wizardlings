#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/Spell.h>
#include <Spells/Spells.h>

class AttackSpell : public Spell
{
public:
	AttackSpell(glm::vec3 pos, glm::vec3 direction, const AttackSpellBase* spellBase);
	~AttackSpell();
	void update(float deltaTime);

private:
	const AttackSpellBase* m_spellBase;
};