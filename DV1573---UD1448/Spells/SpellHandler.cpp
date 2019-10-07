#include "Pch/Pch.h"
#include "SpellHandler.h"

SpellHandler::SpellHandler(glm::vec3 playerPosition, glm::vec3 directionVector)
{
	this->directionVector = directionVector;
	this->spellPos = playerPosition;
	tempSpell = new AttackSpell("Spell", playerPosition, directionVector, 50, 2, "TestSphere.mesh", 0);
	tempEnhanceAttackSpell = new EnhanceAttackSpell("EnhanceSpell", playerPosition, directionVector, 5, 2, "TestCube.mesh", 0, 3, 1);
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
			tempSpell2.setSpellPos(glm::vec3(spellPos.x, spellPos.y - 1.8f, spellPos.z) + directionVector); //-1.8 = spwn point for spell, spell need to be 0 and playerPos is set to (0,1.8,0)
			tempSpell2.translate(tempSpell2.getSpellPos());
			tempSpell2.setDirection(directionVector);
			normalSpell.push_back(tempSpell2);
			tempSpell->setCooldown(1.0f);
		}
	}

	if (type == ENHANCEATTACK)
	{

		int test = 1;

		if (tempEnhanceAttackSpell->getCooldown() <= 0)
		{
			
			EnhanceAttackSpell tempSpell2 = *tempEnhanceAttackSpell;
			directionVector = test * directionVector;
			tempSpell2.setSpellPos(glm::vec3(spellPos.x, spellPos.y - 1.8f, spellPos.z) + directionVector); //-1.8 = spwn point for spell, spell need to be 0 and playerPos is set to (0,1.8,0)
			tempSpell2.translate(tempSpell2.getSpellPos());
			tempSpell2.setDirection(directionVector);
			enhanceAttackSpell.push_back(tempSpell2);
			tempEnhanceAttackSpell->reduceNrOfAttacks(1);
			tempEnhanceAttackSpell->setAttackCooldown(1);
			
			if (tempEnhanceAttackSpell->getNrOfAttacks() <= 0)
			{
				tempEnhanceAttackSpell->setCooldown(2.0);
				tempEnhanceAttackSpell->setNrOfAttacks(3);
			}

			//tempEnhanceAttackSpell->reduceNrOfAttacks(1.0f);
			
			
			//tempEnhanceAttackSpell->setNrOfAttacks(3);

			//EnhanceAttackSpell tempSpell2 = *tempEnhanceAttackSpell;
			//tempSpell2.setSpellPos(glm::vec3(spellPos.x, spellPos.y - 1.8f, spellPos.z) + directionVector); //-1.8 = spwn point for spell, spell need to be 0 and playerPos is set to (0,1.8,0)
			//tempSpell2.translate(tempSpell2.getSpellPos());
			//tempSpell2.setDirection(directionVector);
			//enhanceAttackSpell.push_back(tempSpell2);
			//tempEnhanceAttackSpell->setCooldown(0.5f);

		}
	}

	
}

void SpellHandler::spellUpdate(float deltaTime, TYPE type)
{
	
		for (int i = 0; i < normalSpell.size(); i++)
		{
			normalSpell[i].translate(normalSpell[i].getDirection() * deltaTime * normalSpell[i].getSpellSpeed());
			normalSpell[i].setTravelTime(normalSpell[i].getTravelTime() - 1 * deltaTime);

			if (normalSpell[i].getTravelTime() <= 0)
			{
				normalSpell.erase(normalSpell.begin() + i);
			}
		}
	
	
	
		for (int i = 0; i < enhanceAttackSpell.size(); i++)
		{
			enhanceAttackSpell[i].translate(enhanceAttackSpell[i].getDirection() * deltaTime * enhanceAttackSpell[i].getSpellSpeed());
			enhanceAttackSpell[i].setTravelTime(enhanceAttackSpell[i].getTravelTime() - 1 * deltaTime);

			if (enhanceAttackSpell[i].getTravelTime() <= 0)
			{
				enhanceAttackSpell.erase(enhanceAttackSpell.begin() + i);
			}
		}
	

}

void SpellHandler::spellTest(float deltaTime, TYPE type)
{
	
	if (tempSpell->getCooldown() > 0)
		tempSpell->setCooldown(tempSpell->getCooldown() - 1 * deltaTime);
	
	if (tempEnhanceAttackSpell->getCooldown() > 0)
		tempEnhanceAttackSpell->setCooldown(tempEnhanceAttackSpell->getCooldown() - 1 * deltaTime);

	if (tempEnhanceAttackSpell->getAttackCooldown() > 0)
		tempEnhanceAttackSpell->setAttackCooldown(tempEnhanceAttackSpell->getAttackCooldown() - 1 * deltaTime);
}

void SpellHandler::renderSpell(TYPE type)
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
