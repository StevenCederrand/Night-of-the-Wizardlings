#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/AttackSpell.h>
#include <Spells/EnhanceAttackSpell.h>

enum TYPE { NORMALATTACK, ENHANCEATTACK };

class SpellHandler
{
public:
	SpellHandler(glm::vec3 playerPosition, glm::vec3 directionVector);
	~SpellHandler();
	void createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector, TYPE type);
	void spellUpdate(float deltaTime, TYPE type);
	void spellTest(float deltaTime, TYPE type);
	void renderSpell(TYPE type);

private:
	std::vector<AttackSpell> normalSpell;
	AttackSpell* tempSpell;
	std::vector<EnhanceAttackSpell> enhanceAttackSpell;
	EnhanceAttackSpell* tempEnhanceAttackSpell;

	glm::vec3 directionVector;
	glm::vec3 spellPos;

};