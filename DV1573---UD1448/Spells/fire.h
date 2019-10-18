#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <GameObject/WorldObject.h>
#include <Spells/Spell.h>

class fire : public Spell
{
public:
	fire(glm::vec3 pos);
	fire(std::string name, glm::vec3 pos, glm::vec3 direction, float speed, float travelTime, std::string meshName, float cooldown);
	~fire();

	void updateActiveSpell(float deltaTime);
	void createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector);
	void spellCooldownUpdate(float deltaTime);
	void update(float dt);

	bool isAOE();


private:

};