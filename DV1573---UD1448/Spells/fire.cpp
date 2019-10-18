#include "Pch/Pch.h"
#include "fire.h"

fire::fire(glm::vec3 pos)
	: Spell(pos)
{

}

fire::fire(std::string name, glm::vec3 pos, glm::vec3 direction, float speed, float travelTime, std::string meshName, float cooldown)
	: Spell(name, pos, direction, speed, travelTime, meshName, cooldown)
{

}

fire::~fire()
{

}

void fire::updateActiveSpell(float deltaTime)
{
}

void fire::createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector)
{

}

void fire::spellCooldownUpdate(float deltaTime)
{

}

void fire::update(float dt)
{

}

bool fire::isAOE()
{
	return false;
}
