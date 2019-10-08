#include "Pch/Pch.h"
#include "SpellHandler.h"

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
			//Renderer::getInstance()->render(object);
		}
	
		for (EnhanceAttackSpell object : enhanceAttackSpell)
		{
			object.bindMaterialToShader("Basic_Forward");
			//Renderer::getInstance()->render(object);
		}
	
}
