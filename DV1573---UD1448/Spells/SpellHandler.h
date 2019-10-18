#pragma once
#include <Pch/Pch.h>
#include <Spells/Spells.h>
#include <GameObject/GameObject.h>
#include <Spells/AttackSpell.h>
#include <Spells/EnhanceAttackSpell.h>
#include <Spells/Spells.h>


enum SPELLTYPE { NORMALATTACK, ENHANCEHANDLER, ENHANCEATTACK, FLAMESTRIKE };

class SpellHandler
{
public:
	SpellHandler();
	void initAttackSpell();
	void initEnhanceSpell();


	~SpellHandler();
	void createSpell(glm::vec3 spellPos, glm::vec3 directionVector, SPELLTYPE type);
	void spellUpdate(float deltaTime);
	const AttackSpellBase& getSpellBase(SPELLTYPE spelltype);
	const Spell& getSpell(int index) const { return *spells[index]; }
	const std::vector<Spell*>& getSpells() const { return spells; }
	void renderSpell();
	//bool isSpellReadyToCast(SPELLTYPE type);


private:
	std::vector<Spell*> spells;

	// The base for all basic attack spells
	AttackSpellBase* attackBase;
	EnhanceHanderSpellBase* enhanceHandlerBase;
	EnhanceAtkSpellBase* enhanceatkBase;



	//std::vector<AOEAttack> m_flamestrike;
	//AOEAttack* m_tempFlamestrike;
	//fire* tempFire;
	//fire* m_fire;


	//other things
	glm::vec3 m_directionVector;
	glm::vec3 m_spellPos;
	glm::vec3 m_saveValues = glm::vec3(0,0,0);
	SPELLTYPE m_spellType;

	bool test123 = false;

};