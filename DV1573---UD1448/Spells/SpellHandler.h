#pragma once
#include <Pch/Pch.h>
#include <Spells/Spells.h>
#include <GameObject/GameObject.h>
#include <Spells/AttackSpell.h>
#include <Spells/EnhanceAttackSpell.h>
#include <Spells/Spells.h>

enum TYPE { NORMALATTACK, ENHANCEATTACK, FLAMESTRIKE };



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
	void renderSpell();
	//bool isSpellReadyToCast(TYPE type);

	void setType(TYPE type);
	TYPE getType();

private:
	std::vector<Spell*> spells;
	std::vector<EnhanceAttackSpell> enhanceAttackSpell;

	// The base for all basic attack spells
	AttackSpellBase* attackBase;
	//all spells
	std::vector<AttackSpell> m_normalSpell;
	AttackSpell* m_tempSpell;
	std::vector<EnhanceAttackSpell> m_enhanceAttackSpell;
	EnhanceAttackSpell* m_tempEnhanceAttackSpell;
	std::vector<AOEAttack> m_flamestrike;
	AOEAttack* m_tempFlamestrike;
	fire* tempFire;
	fire* m_fire;



	//other things
	glm::vec3 m_directionVector;
	glm::vec3 m_spellPos;
	glm::vec3 m_saveValues = glm::vec3(0,0,0);
	TYPE m_spellType;

	bool test123 = false;

};