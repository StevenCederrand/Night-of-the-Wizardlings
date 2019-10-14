#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/Spell.h>

class EnhanceAttackSpell : public Spell
{
public:
	EnhanceAttackSpell(glm::vec3 pos);
	EnhanceAttackSpell(std::string name, glm::vec3 pos, glm::vec3 m_direction, float speed, float m_travelTime, std::string meshName, float cooldown, float nrOfEnhancedAttacks);
	~EnhanceAttackSpell();

	float getThreeAttacks();
	void setThreeAttacks(float nrOfEnhancedAttacks);
	void updateActiveSpell(float deltaTime);
	void spellCooldownUpdate(float deltaTime);
	void createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector);

	void update(float deltaTime);


private:

	float nrOfEnhancedAttacks = 0;
};