#include "Pch/Pch.h"
#include "SpellHandler.h"

SpellHandler::SpellHandler(glm::vec3 playerPosition, glm::vec3 directionVector)
{
	this->m_directionVector = directionVector;
	m_tempSpell = new AttackSpell("Spell", playerPosition, directionVector, 50, 2, "TestSphere.mesh", 0);
	m_tempEnhanceAttackSpell = new EnhanceAttackSpell("EnhanceSpell", playerPosition, directionVector, 150, 1, "TestCube.mesh", 0, 3, 1, 0);
	m_tempFlamestrike = new AOEAttack("Flamestrike", playerPosition, directionVector, 10, 20, "TestCube.mesh", 0);
	tempFire = new fire("Flamestrike", playerPosition, directionVector, 10, 20, "TestFireCylinder.mesh", 0);
}

SpellHandler::~SpellHandler()
{
	delete m_tempSpell;
	delete m_tempEnhanceAttackSpell;
	delete m_tempFlamestrike;
	
}

bool SpellHandler::createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector, TYPE type)
{
	bool spellIsOver = false;
	if (type == NORMALATTACK)
	{
		if(m_tempSpell->getCooldown() <= 0)
		{
			AttackSpell tempSpell2 = *m_tempSpell;
			tempSpell2.createSpell(deltaTime, spellPos, directionVector);
			m_normalSpell.push_back(tempSpell2);
			m_tempSpell->setCooldown(1.0f);
		}
	}

	if (type == ENHANCEATTACK)
	{
		if (m_tempEnhanceAttackSpell->getCooldown() <= 0)
		{
			if (m_tempEnhanceAttackSpell->getAttackCooldown() <= 0)
			{
				EnhanceAttackSpell tempSpell2 = *m_tempEnhanceAttackSpell;
				tempSpell2.createSpell(deltaTime, spellPos, directionVector);
				m_enhanceAttackSpell.push_back(tempSpell2);
				m_tempEnhanceAttackSpell->setAttackCooldown(0.3f);
				m_tempEnhanceAttackSpell->reduceNrOfAttacks(1.0f);
			}
			if (m_tempEnhanceAttackSpell->getNrOfAttacks() <= 0)
			{
				m_tempEnhanceAttackSpell->setCooldown(10.0f);
				m_tempEnhanceAttackSpell->setNrOfAttacks(3);

				//-----Return true if the spell is done in order to get the normal attack back-----//
				m_tempSpell->setCooldown(1.0f);
				setType(NORMALATTACK);
				spellIsOver = true;
			}
		}

	}	

	if (type == FLAMESTRIKE)
	{
		if (m_tempFlamestrike->getCooldown() <= 0)
		{
			AOEAttack tempSpell2 = *m_tempFlamestrike;
			tempSpell2.createSpell(deltaTime, spellPos, directionVector);
			m_flamestrike.push_back(tempSpell2);
			m_tempFlamestrike->setCooldown(7.0f);

			m_tempSpell->setCooldown(1.0f);
			setType(NORMALATTACK);
			spellIsOver = true;
		}
	}
	return spellIsOver;
}

void SpellHandler::spellUpdate(float deltaTime)
{
		for (int i = 0; i < m_normalSpell.size(); i++)
		{
			m_normalSpell[i].updateActiveSpell(deltaTime);
			if (m_normalSpell[i].getTravelTime() <= 0)
			{
				m_normalSpell.erase(m_normalSpell.begin() + i);
			}
		}
	
		for (int i = 0; i < m_enhanceAttackSpell.size(); i++)
		{
			m_enhanceAttackSpell[i].updateActiveSpell(deltaTime);
			if (m_enhanceAttackSpell[i].getTravelTime() <= 0)
			{
				m_enhanceAttackSpell.erase(m_enhanceAttackSpell.begin() + i);
			}
		}

		for (int i = 0; i < m_flamestrike.size(); i++)
		{

			m_flamestrike[i].updateActiveSpell(deltaTime);
			
			std::cout << m_flamestrike[i].getSpellPos().x << " " << m_flamestrike[i].getSpellPos().y << " " << m_flamestrike[i].getSpellPos().z << std::endl;
			tempFire->setWorldPosition(m_flamestrike[i].getSpellPos());
			
			if (m_flamestrike[i].isAOE())
			{
				tempFire->translate(m_flamestrike[i].getSpellPos());
			}
			

			if (m_flamestrike[i].getTravelTime() <= 0)
			{
				m_flamestrike.erase(m_flamestrike.begin() + i);
			}
		}
}

void SpellHandler::spellCooldown(float deltaTime)
{
	m_tempSpell->spellCooldownUpdate(deltaTime);
	m_tempEnhanceAttackSpell->spellCooldownUpdate(deltaTime);
	m_tempEnhanceAttackSpell->attackCooldownUpdate(deltaTime);
	m_tempFlamestrike->spellCooldownUpdate(deltaTime);
}

void SpellHandler::renderSpell()
{
	
		for (AttackSpell object : m_normalSpell)
		{
			object.bindMaterialToShader("Basic_Forward");
			Renderer::getInstance()->render(object);
		}
	
		for (EnhanceAttackSpell object : m_enhanceAttackSpell)
		{
			object.bindMaterialToShader("Basic_Forward");
			Renderer::getInstance()->render(object);
		}

		for (AOEAttack object : m_flamestrike)
		{
			object.bindMaterialToShader("Basic_Forward");
			Renderer::getInstance()->render(object);
		}
		for (int i = 0; i < m_flamestrike.size(); i++)
		{
			if (m_flamestrike[i].isAOE())
			{
				tempFire->bindMaterialToShader("Basic_Forward");
				Renderer::getInstance()->render(*tempFire);
			}
		}
	
}

void SpellHandler::setType(TYPE type)
{
	m_spellType = type;
}



TYPE SpellHandler::getType()
{
	return m_spellType;
}


