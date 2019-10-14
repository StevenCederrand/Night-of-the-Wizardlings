#include "Pch/Pch.h"
#include "AOEAttack.h"

AOEAttack::AOEAttack(glm::vec3 pos)
	:Spell(pos)
{
}

AOEAttack::AOEAttack(std::string name, glm::vec3 pos, glm::vec3 direction, float speed, float travelTime, std::string meshName, float cooldown)
	: Spell(name, pos, direction, speed, travelTime, meshName, cooldown)
{
	fireIsGone = 0;
	this->pos = pos;
	loadMesh("TestFireCylinder.mesh");
}

AOEAttack::~AOEAttack()
{

}

void AOEAttack::updateActiveSpell(float deltaTime)
{
	if (m_updateSpellPos == true)
	{
		setSpellPos(getDirection() * deltaTime * getSpellSpeed());
		newVer += getSpellPos();
		setSpellPos(newVer);
	}
	
	//updateSpellPos(getSpellPos());

	setDirection(getDirection() + deltaTime * gravityVector);

	if (getSpellPos().y >= 0)
	{
		translate(getDirection());
	}
		
	std::cout << getSpellPos().x << " " << getSpellPos().y << " " << getSpellPos().z << std::endl;

	setTravelTime(getTravelTime() - 1 * deltaTime);

	//std::cout << getSpellPos().x << " " << getSpellPos().y << " " << getSpellPos().z << std::endl;

	//AOE
	if (getSpellPos().y <= 0)
	{
		m_updateSpellPos = false;

		radiusVector = getSpellPos() + glm::vec3(5, 0, 0);

		////std::cout << "Fire! You are burning" << std::endl;
		//if (loadFire == true)
		//{		
		//	//tempVer = ;
		//	tempFire->loadMesh("TestFireCylinder.mesh");
		//	tempFire->setWorldPosition(glm::vec3(getSpellPos()));
		//	tempFire->translate(glm::vec3(getSpellPos()));
		//	std::cout << pos.x << " " << pos.y << " " << pos.z << std::endl;
		//	loadFire = false;
		//}
		//tempFire->bindMaterialToShader("Basic_Forward");
		//Renderer::getInstance()->render(*tempFire);
		////test123 = true;
		fireIsGone += 1 * deltaTime;
		if (fireIsGone >= 5)
			setTravelTime(0);

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

bool AOEAttack::isAOE()
{
	if (m_updateSpellPos == false)
		return true;
	else
		return false;
}
