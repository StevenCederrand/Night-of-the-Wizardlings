#include "Pch/Pch.h"
#include "SpellHandler.h"

SpellHandler::SpellHandler(glm::vec3 playerPosition, glm::vec3 directionVector)
{
	this->directionVector = directionVector;
	tempSpell = new AttackSpell("Spell", playerPosition, directionVector, 50, 2, "TestSphere.mesh", 0);
	tempEnhanceAttackSpell = new EnhanceAttackSpell("EnhanceSpell", playerPosition, directionVector, 150, 1, "TestCube.mesh", 0, 3, 1, 0);
	tempFlamestrike = new AOEAttack("Flamestrike", playerPosition, directionVector, 10, 20, "TestCube.mesh", 0);

}

SpellHandler::~SpellHandler()
{
	delete tempSpell;
	delete tempEnhanceAttackSpell;
	delete tempFlamestrike;
}

bool SpellHandler::createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector, TYPE type)
{
	bool spellIsOver = false;
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
		if (tempEnhanceAttackSpell->getCooldown() <= 0)
		{
			if (tempEnhanceAttackSpell->getAttackCooldown() <= 0)
			{
				EnhanceAttackSpell tempSpell2 = *tempEnhanceAttackSpell;
				tempSpell2.createSpell(deltaTime, spellPos, directionVector);
				enhanceAttackSpell.push_back(tempSpell2);
				tempEnhanceAttackSpell->setAttackCooldown(0.3f);
				tempEnhanceAttackSpell->reduceNrOfAttacks(1.0f);
			}
			if (tempEnhanceAttackSpell->getNrOfAttacks() <= 0)
			{
				tempEnhanceAttackSpell->setCooldown(10.0f);
				tempEnhanceAttackSpell->setNrOfAttacks(3);

				//-----Return true if the spell is done in order to get the normal attack back-----//
				tempSpell->setCooldown(1.0f);
				setType(NORMALATTACK);
				spellIsOver = true;
			}
		}

	}	

	if (type == FLAMESTRIKE)
	{
		if (tempFlamestrike->getCooldown() <= 0)
		{
			AOEAttack tempSpell2 = *tempFlamestrike;
			tempSpell2.createSpell(deltaTime, spellPos, directionVector);
			flamestrike.push_back(tempSpell2);
			tempFlamestrike->setCooldown(1.0f);

			tempSpell->setCooldown(1.0f);
			setType(NORMALATTACK);
			spellIsOver = true;
		}
	}
	return spellIsOver;
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

		for (int i = 0; i < flamestrike.size(); i++)
		{

			flamestrike[i].updateActiveSpell(deltaTime);
			if (flamestrike[i].getTravelTime() <= 0)
			{
				flamestrike.erase(flamestrike.begin() + i);
			}
		}


}

void SpellHandler::spellCooldown(float deltaTime)
{
	tempSpell->spellCooldownUpdate(deltaTime);
	tempEnhanceAttackSpell->spellCooldownUpdate(deltaTime);
	tempEnhanceAttackSpell->attackCooldownUpdate(deltaTime);
	tempFlamestrike->spellCooldownUpdate(deltaTime);
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

		for (AOEAttack object : flamestrike)
		{
			object.bindMaterialToShader("Basic_Forward");
			Renderer::getInstance()->render(object);
		}
	
}

void SpellHandler::setType(TYPE type)
{
	this->spellType = type;
}



TYPE SpellHandler::getType()
{
	return this->spellType;
}


