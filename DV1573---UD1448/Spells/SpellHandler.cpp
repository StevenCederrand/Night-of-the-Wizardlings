#include "Pch/Pch.h"
#include "SpellHandler.h"
#include <Networking/Client.h>

SpellHandler::SpellHandler(glm::vec3 playerPosition, glm::vec3 directionVector)
{
	this->directionVector = directionVector;
	this->spellPos = playerPosition;
	tempSpell = new AttackSpell("Spell", playerPosition, directionVector, 50, 2, "TestSphere.mesh", 0);
	tempEnhanceAttackSpell = new EnhanceAttackSpell("EnhanceSpell", playerPosition, directionVector, 10, 4, "TestCube.mesh", 0, 3);
}

SpellHandler::~SpellHandler()
{
	delete tempSpell;
	delete tempEnhanceAttackSpell;
}

void SpellHandler::createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector, TYPE type)
{
	if (type == NORMALATTACK)
	{
		if(tempSpell->getCooldown() <= 0)
		{
			AttackSpell tempSpell2 = *tempSpell;
			tempSpell2.createSpell(deltaTime, spellPos, directionVector);
			normalSpell.push_back(tempSpell2);
			tempSpell->setCooldown(1.0f);
		}
	}

	if (type == ENHANCEATTACK)
	{
		if (tempEnhanceAttackSpell->getCooldown() <= 0)//&& tempEnhanceAttackSpell->getThreeAttacks() <= 0)
		{
			EnhanceAttackSpell tempSpell2 = *tempEnhanceAttackSpell;
			tempSpell2.createSpell(deltaTime, spellPos, directionVector);
			enhanceAttackSpell.push_back(tempSpell2);
			tempEnhanceAttackSpell->setCooldown(5.0f);
		}
	}	
}

void SpellHandler::spellUpdate(float deltaTime)
{
	for (int i = 0; i < normalSpell.size(); i++)
	{
		normalSpell[i].updateActiveSpell(deltaTime);
		if (normalSpell[i].getTravelTime() <= 0)
		{
			normalSpell.erase(normalSpell.begin() + i);
		}
	}
	
	for (int i = 0; i < enhanceAttackSpell.size(); i++)
	{
	
		enhanceAttackSpell[i].updateActiveSpell(deltaTime);
		if (enhanceAttackSpell[i].getTravelTime() <= 0)
		{
			enhanceAttackSpell.erase(enhanceAttackSpell.begin() + i);
		}
	}
	spellCollisionCheck();
}

void SpellHandler::spellCooldown(float deltaTime)
{
	
	tempSpell->spellCooldownUpdate(deltaTime);
	tempEnhanceAttackSpell->spellCooldownUpdate(deltaTime);
}

void SpellHandler::renderSpell()
{
	

		for (AttackSpell object : normalSpell)
		{
			object.bindMaterialToShader("Basic_Forward");
			Renderer::getInstance()->render(object);
		}
	
		for (EnhanceAttackSpell object : enhanceAttackSpell)
		{
			object.bindMaterialToShader("Basic_Forward");
			Renderer::getInstance()->render(object);
		}
	
}

void SpellHandler::spellCollisionCheck()
{
	//get the list of att the players on the network
	auto& list = Client::getInstance()->getNetworkPlayersREF().getPlayersREF();


	for (size_t i = 0; i < list.size(); i++)
	{
		glm::vec3 playerPos = list[i]->data.position;
		list[i]->data.rotation;
		//create a box, obb or AABB? from the player position
		for (int i = 0; i < normalSpell.size(); i++) {
			glm::vec3 spherePos = normalSpell.at(i).getTransform().position;
			float sphereRadius = 2.0f;




		//create a sphere from the spells position,		maybe have a capsule?
		}

		//check collision between the sphere and the box


	}
	//check the collision with your spells and you!
}
