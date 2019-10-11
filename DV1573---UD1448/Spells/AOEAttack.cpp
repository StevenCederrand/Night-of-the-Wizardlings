#include "Pch/Pch.h"
#include "AOEAttack.h"

AOEAttack::AOEAttack(glm::vec3 pos)
	:Spell(pos)
{
}

AOEAttack::AOEAttack(std::string name, glm::vec3 pos, glm::vec3 direction, float speed, float travelTime, std::string meshName, float cooldown)
	: Spell(name, pos, direction, speed, travelTime, meshName, cooldown)
{

}

AOEAttack::~AOEAttack()
{

}

void AOEAttack::updateActiveSpell(float deltaTime)
{
	setSpellPos(getDirection() * deltaTime * getSpellSpeed());
	newVer += getSpellPos();
	setSpellPos(newVer);

	setDirection(getDirection() + deltaTime * gravityVector);

	if(getSpellPos().y >= 0)
		translate(getDirection());

	setTravelTime(getTravelTime() - 1 * deltaTime);
	std::cout << getSpellPos().x << " " << getSpellPos().y << " " << getSpellPos().z << std::endl;

	//AOE
	if (getSpellPos().y <= 0)
	{
		std::cout << "Fire! You are burning" << std::endl;

		setTravelTime(0);
		//std::cout << getSpellPos().x << " " << getSpellPos().y << " " << getSpellPos().z << std::endl;
		//setWorldPosition(getSpellPos());
	}
}

void AOEAttack::createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector)
{
	setSpellPos(glm::vec3(spellPos.x, spellPos.y - 1.8f, spellPos.z) + directionVector);
	translate(getSpellPos());
	setDirection(directionVector);

}

void AOEAttack::spellCooldownUpdate(float deltaTime)
{
	if (getCooldown() > 0)
		setCooldown(getCooldown() - 1 * deltaTime);
}

void AOEAttack::update(float dt)
{

}
