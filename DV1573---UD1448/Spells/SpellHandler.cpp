#include "Pch/Pch.h"
#include "SpellHandler.h"
#include <Networking/Client.h>

SpellHandler::SpellHandler(glm::vec3 playerPosition, glm::vec3 directionVector, BulletPhysics* bp)
{
	this->directionVector = directionVector;
	this->spellPos = playerPosition;
	tempSpell = new AttackSpell("Spell", playerPosition, directionVector, 50, 2, "TestSphere.mesh", 0);
	tempEnhanceAttackSpell = new EnhanceAttackSpell("EnhanceSpell", playerPosition, directionVector, 10, 4, "TestCube.mesh", 0, 3);

	m_bp = bp;
}

SpellHandler::~SpellHandler()
{
	delete tempSpell;
	delete tempEnhanceAttackSpell;
}

void SpellHandler::createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector, TYPE type)
{
	CollisionObject obj = sphere;
	btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.x);
	if (type == NORMALATTACK)
	{
		if(tempSpell->getCooldown() <= 0)
		{
			//bullet create
			m_BulletNormalSpell.emplace_back(
				m_bp->createObject(obj, 1.0f, spellPos+directionVector*2, glm::vec3(1.0f, 0.0f, 0.0f)));
			
			AttackSpell tempSpell2 = *tempSpell;
			tempSpell2.createSpell(deltaTime, spellPos+directionVector*2, directionVector);
			normalSpell.push_back(tempSpell2);
			tempSpell->setCooldown(1.0f);

			//bullet
			int size = m_BulletNormalSpell.size();
			float spellSpeed = tempSpell2.getSpellSpeed();
			m_BulletNormalSpell.at(size - 1)->setLinearVelocity(direction*deltaTime*spellSpeed);
			m_BulletNormalSpell.at(size - 1)->setGravity(btVector3(0.0f, 0.0f, 0.0f));
			m_BulletNormalSpell.at(size - 1)->setUserPointer(m_BulletNormalSpell.at(size - 1));
		}
	}

	if (type == ENHANCEATTACK)
	{
		if (tempEnhanceAttackSpell->getCooldown() <= 0) //&& tempEnhanceAttackSpell->getThreeAttacks() <= 0)
		{
			m_BulletEnhanceAttackSpell.emplace_back(
				m_bp->createObject(obj, 1.0f, spellPos+directionVector*2, glm::vec3(1.0f, 0.0f, 0.0f)));


			EnhanceAttackSpell tempSpell2 = *tempEnhanceAttackSpell;
			tempSpell2.createSpell(deltaTime, spellPos, directionVector);
			enhanceAttackSpell.push_back(tempSpell2);
			tempEnhanceAttackSpell->setCooldown(5.0f);

			int size = m_BulletEnhanceAttackSpell.size();
			float spellSpeed = tempSpell2.getSpellSpeed();
			m_BulletEnhanceAttackSpell.at(size - 1)->setLinearVelocity(direction);
			m_BulletEnhanceAttackSpell.at(size - 1)->setGravity(btVector3(0.0f, 0.0f, 0.0f));

		}
	}	
}

void SpellHandler::spellUpdate(float deltaTime)
{
	//deltaTime = 0.005f;
	for (int i = 0; i < normalSpell.size(); i++)
	{
		normalSpell[i].updateActiveSpell(deltaTime, m_BulletNormalSpell.at(i));
		glm::vec3 pos = normalSpell[i].getSpellPos();
		logTrace("\n\npos for the spell: ");
		logTrace(pos.x);
		logTrace(pos.y);
		logTrace(pos.z);

		if (normalSpell[i].getTravelTime() <= 0)
		{
			normalSpell.erase(normalSpell.begin() + i);
			m_BulletNormalSpell.erase(m_BulletNormalSpell.begin() + i);

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
	
	//BEGIN TEST=========================================== BEGIN TEST
		glm::vec3 playerPos = glm::vec3(0.0f, 1.0f, -2.0f);

		glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
		std::vector<glm::vec3> axis;

		axis.emplace_back(xAxis);
		axis.emplace_back(yAxis);
		axis.emplace_back(zAxis);
		//NORMAL spells
	for (int i = 0; i < normalSpell.size(); i++) {

		glm::vec3 spellPos = normalSpell.at(i).getTransform().position;
		specificSpellCollision(spellPos, playerPos, axis);
	}
	//ENCHANCEATTACK spells
	for (int i = 0; i < enhanceAttackSpell.size(); i++)
	{
		glm::vec3 spellPos = enhanceAttackSpell.at(i).getTransform().position;
		specificSpellCollision(spellPos, playerPos, axis);
	}
	
	//END TEST=========================================== END TEST

	
	//get the list of att the players on the network
	auto& list = Client::getInstance()->getNetworkPlayersREF().getPlayersREF();

	for (size_t i = 0; i < list.size(); i++)
	{
		glm::vec3 playerPos = list[i]->data.position;
		list[i]->data.rotation;

		//create the axis and rotate them
		glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
		std::vector<glm::vec3> axis;

		glm::rotateX(xAxis, list[i]->data.rotation.x);
		glm::rotateY(xAxis, list[i]->data.rotation.y);
		glm::rotateZ(xAxis, list[i]->data.rotation.z);

		axis.emplace_back(xAxis);
		axis.emplace_back(yAxis);
		axis.emplace_back(zAxis);
		
		//create a box, obb or AABB? from the player position
		for (int i = 0; i < normalSpell.size(); i++) {
			glm::vec3 spherePos = normalSpell.at(i).getTransform().position;
			glm::vec3 closestPoint = OBBclosestPoint(spherePos, axis,playerPos);
			float sphereRadius = 1.0f;
			glm::vec3 v = closestPoint - spherePos;

			if (glm::dot(v, v) <= sphereRadius * sphereRadius)
			{
				//COLLISION!
				logTrace("COLLISION spell and player");
			}			
		}
	}
	//check the collision with your spells and you!
}

bool SpellHandler::specificSpellCollision(glm::vec3 spellPos, glm::vec3 playerPos, std::vector<glm::vec3>& axis)
{
	bool collision = false;
	float sphereRadius = 0.6f;

	glm::vec3 closestPoint = OBBclosestPoint(spellPos, axis, playerPos);
	glm::vec3 v = closestPoint - spellPos;

	if (glm::dot(v, v) <= sphereRadius * sphereRadius)
	{
		//COLLISION!
		logTrace("COLLISION spell and player");
		collision = true;
	}
	return collision;
}

glm::vec3 SpellHandler::OBBclosestPoint(glm::vec3& spherePos, std::vector<glm::vec3>& axis, glm::vec3& playerPos)
{
	float boxSize = 0.25f;
	//closest point on obb
	glm::vec3 boxPoint = playerPos;
	glm::vec3 ray = glm::vec3(spherePos - playerPos);

	for (int j = 0; j < 3; j++) {
		float distance = glm::dot(ray, axis.at(j));
		float distance2 = 0;

		if (distance > boxSize)
			distance2 = boxSize;

		if (distance < -boxSize)
			distance2 = -boxSize;

		
		boxPoint += distance2 * axis.at(j);
	}

	return boxPoint;
}
