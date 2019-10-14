#pragma once
#include <Pch/Pch.h>
#include "AttackSpell.h"

AttackSpell::AttackSpell(glm::vec3 pos)
	: Spell(pos)
{
}

AttackSpell::AttackSpell(std::string name, glm::vec3 pos, glm::vec3 direction, float speed, float travelTime, std::string meshName, float cooldown)
	: Spell(name, pos, direction, speed, travelTime, meshName, cooldown)
{
}

AttackSpell::~AttackSpell()
{
}

void AttackSpell::updateActiveSpell(float deltaTime, btRigidBody* body)
{
	

	//pos is for the spell to have the bodys position
	glm::vec3 pos = glm::vec3(
		body->getWorldTransform().getOrigin().getX(),
		body->getWorldTransform().getOrigin().getY(),
		body->getWorldTransform().getOrigin().getZ());

	//change the bodys velocity every frame based on dt
	btVector3 pos2 = btVector3(
		getDirection().x * deltaTime * getSpellSpeed(),
		getDirection().y * deltaTime * getSpellSpeed(),
		getDirection().z * deltaTime * getSpellSpeed())*10;


	logTrace("\n\npos: ");
	logTrace(pos.x /deltaTime /getSpellSpeed() /10);
	logTrace(pos.y);
	logTrace(pos.z);
	

	/*logTrace("\npos2: ");
	logTrace(pos2.getX());
	logTrace(pos2.getY());
	logTrace(pos2.getZ());*/

	body->setLinearVelocity(pos2);

	
	//translate(getDirection() * deltaTime * getSpellSpeed());
	setWorldPosition(pos);
	setTravelTime(getTravelTime() - 1 * deltaTime);

	/*logTrace("\npos: " );
	logTrace(pos.x);
	logTrace(pos.y);
	logTrace(pos.z);*/

}

void AttackSpell::createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector)
{
	setSpellPos(glm::vec3(spellPos.x, spellPos.y - 1.8f, spellPos.z) + directionVector); //-1.8 = spwn point for spell, spell need to be 0 and playerPos is set to (0,1.8,0)
	translate(getSpellPos());
	setDirection(directionVector);
}

void AttackSpell::spellCooldownUpdate(float deltaTime)
{
	if (getCooldown() > 0)
		setCooldown(getCooldown() - 1 * deltaTime);
}

void AttackSpell::update(float dt)
{
}


