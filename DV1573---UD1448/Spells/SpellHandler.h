#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/AttackSpell.h>
#include <Spells/EnhanceAttackSpell.h>
#include <Spells/Spells.h>

enum TYPE { NORMALATTACK, ENHANCEATTACK };

class SpellHandler
{
public:
	SpellHandler();
	void initAttackSpell();
	~SpellHandler();
	void createSpell(glm::vec3 spellPos, glm::vec3 directionVector, TYPE type);
	void spellUpdate(float deltaTime);
	const AttackSpellBase& getAttackSpellBase() const { return *attackBase; }
	const Spell& getSpell(int index) const { return *spells[index]; }
	const std::vector<Spell*>& getSpells() const { return spells; }

private:
	std::vector<Spell*> spells;
	std::vector<EnhanceAttackSpell> enhanceAttackSpell;

	// The base for all basic attack spells
	AttackSpellBase* attackBase;


};