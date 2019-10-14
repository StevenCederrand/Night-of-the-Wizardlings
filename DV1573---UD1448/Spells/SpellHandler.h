#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/AttackSpell.h>
#include <Spells/EnhanceAttackSpell.h>
#include <Spells/AOEAttack.h>
#include <Spells/fire.h>

enum TYPE { NORMALATTACK, ENHANCEATTACK, FLAMESTRIKE };

class SpellHandler
{
public:
	SpellHandler(glm::vec3 playerPosition, glm::vec3 directionVector);
	~SpellHandler();
	bool createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector, TYPE type);
	void spellUpdate(float deltaTime);
	void spellCooldown(float deltaTime);
	void renderSpell();
	//bool isSpellReadyToCast(TYPE type);

	void setType(TYPE type);
	TYPE getType();

private:
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