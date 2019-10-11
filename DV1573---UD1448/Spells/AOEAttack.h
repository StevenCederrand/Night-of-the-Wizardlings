#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/Spell.h>

class AOEAttack : public Spell
{
public:
	AOEAttack(glm::vec3 pos);
	AOEAttack(std::string name, glm::vec3 pos, glm::vec3 direction, float speed, float travelTime, std::string meshName, float cooldown);
	~AOEAttack();

	void updateActiveSpell(float deltaTime);
	void createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector);
	void spellCooldownUpdate(float deltaTime);
	void update(float dt);


private:
	glm::vec3 gravityVector = glm::vec3(0,-1,0);
	glm::vec3 newVer = glm::vec3(0, 0, 0);
};