#pragma once
#include <Pch/Pch.h>
#include <Spells/Spells.h>
#include <GameObject/GameObject.h>
#include <Spells/AttackSpell.h>
#include <Spells/EnhanceAttackSpell.h>
#include "SpellTypes.h"




class SpellHandler
{
public:
	SpellHandler(glm::vec3 playerPosition, glm::vec3 directionVector);
	~SpellHandler();
	void createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector, SPELL_TYPE type);
	void spellUpdate(float deltaTime);
	void spellCooldown(float deltaTime);
	void renderSpell();

private:
	const uint64_t getUniqueID();

private:

	std::vector<AttackSpell> normalSpell;
	AttackSpell* tempSpell;
	std::vector<EnhanceAttackSpell> enhanceAttackSpell;
	EnhanceAttackSpell* tempEnhanceAttackSpell;

	glm::vec3 directionVector;
	glm::vec3 spellPos;

};