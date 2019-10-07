#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/Spell.h>

class EnhanceAttackSpell : public Spell
{
public:
	EnhanceAttackSpell(glm::vec3 pos);
	EnhanceAttackSpell(std::string name, glm::vec3 pos, glm::vec3 direction, float speed, float travelTime, std::string meshName, float cooldown, float nrOfEnhancedAttacks, float attackCooldown);
	~EnhanceAttackSpell();

	float getNrOfAttacks();
	float getAttackCooldown();

	void setNrOfAttacks(float nrOfEnhancedAttacks);
	void reduceNrOfAttacks(float nrOfEnhancedAttacks);
	void setAttackCooldown(float attackCooldown);

	void update(float dt);

private:

	float nrOfAttacks = 3;
	float attackCooldown = 0;
};