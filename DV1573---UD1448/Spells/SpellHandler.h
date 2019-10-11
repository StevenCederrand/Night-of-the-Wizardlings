#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/AttackSpell.h>
#include <Spells/EnhanceAttackSpell.h>
#include <Spells/AOEAttack.h>

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

	void setType(TYPE type);
	TYPE getType();

private:
	//all spells
	std::vector<AttackSpell> normalSpell;
	AttackSpell* tempSpell;
	std::vector<EnhanceAttackSpell> enhanceAttackSpell;
	EnhanceAttackSpell* tempEnhanceAttackSpell;
	std::vector<AOEAttack> flamestrike;
	AOEAttack* tempFlamestrike;

	//other things
	glm::vec3 directionVector;
	glm::vec3 spellPos;
	TYPE spellType;


};