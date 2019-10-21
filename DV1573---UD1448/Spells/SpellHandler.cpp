#include "Pch/Pch.h"
#include "SpellHandler.h"
#include <Networking/Client.h>
#include <Loader/BGLoader.h>


SpellHandler::SpellHandler(BulletPhysics * bp)
{
	attackBase = nullptr;
	enhanceAtkBase = nullptr;
	initAttackSpell();
	m_bp = bp;
	initEnhanceSpell();
}

void SpellHandler::initAttackSpell()
{
	attackBase = new AttackSpellBase();
	attackBase->m_mesh = new Mesh();
	attackBase->m_material = new Material();

	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + "TestSphere.mesh");
	attackBase->m_mesh->saveFilePath(tempLoader.GetFileName(), 0);
	attackBase->m_mesh->nameMesh(tempLoader.GetMeshName());
	attackBase->m_mesh->setUpMesh(tempLoader.GetVertices(), tempLoader.GetFaces());
	attackBase->m_mesh->setUpBuffers();

	const Material& newMaterial = tempLoader.GetMaterial();
	attackBase->m_material->ambient = newMaterial.ambient;
	attackBase->m_material->diffuse = newMaterial.diffuse;
	attackBase->m_material->name = newMaterial.name;
	attackBase->m_material->specular = newMaterial.specular;
	tempLoader.Unload();

	attackBase->m_material->diffuse = glm::vec3(0.65f, 1.0f, 1.0f);
	attackBase->m_material->ambient = glm::vec3(0.65f, 1.0f, 1.0f);

	attackBase->m_damage = 34;
	attackBase->m_speed = 25;
	attackBase->m_coolDown = 1;
	attackBase->m_lifeTime = 5;
	attackBase->m_maxBounces = 3;
}


void SpellHandler::initEnhanceSpell()
{
	enhanceAtkBase = new AttackSpellBase();
	enhanceAtkBase->m_mesh = new Mesh();
	enhanceAtkBase->m_material = new Material();


	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + "TestSphere.mesh");
	enhanceAtkBase->m_mesh = new Mesh();
	enhanceAtkBase->m_mesh->saveFilePath(tempLoader.GetFileName(), 0);
	enhanceAtkBase->m_mesh->nameMesh(tempLoader.GetMeshName());
	enhanceAtkBase->m_mesh->setUpMesh(tempLoader.GetVertices(), tempLoader.GetFaces());
	enhanceAtkBase->m_mesh->setUpBuffers();

	const Material& newMaterial = tempLoader.GetMaterial();
	enhanceAtkBase->m_material->ambient = newMaterial.ambient;
	enhanceAtkBase->m_material->diffuse = newMaterial.diffuse;
	enhanceAtkBase->m_material->name = newMaterial.name;
	enhanceAtkBase->m_material->specular = newMaterial.specular;
	tempLoader.Unload();

	enhanceAtkBase->m_damage = 34;
	enhanceAtkBase->m_speed = 100;
	enhanceAtkBase->m_coolDown = 1;
	enhanceAtkBase->m_lifeTime = 5;
	enhanceAtkBase->m_maxBounces = 3;
}

void SpellHandler::initnrOfRigidBodys()
{
	m_nrOfOtherrigidBodys = m_bp->getDynamicsWorld()->getNumCollisionObjects();
}

SpellHandler::~SpellHandler()
{
	if (attackBase)
		delete attackBase;
	for (Spell* element : spells)
		delete element;
	spells.clear();
}



void SpellHandler::createSpell(glm::vec3 spellPos, glm::vec3 directionVector, SPELL_TYPE type)
{

	CollisionObject obj = sphere;
	if (type == NORMALATTACK)
	{
		auto spell = new AttackSpell(spellPos, directionVector, attackBase);
		spell->setUniqueID(getUniqueID());
		Client::getInstance()->createSpellOnNetwork(*spell);
		spells.emplace_back(spell);
		Renderer::getInstance()->submit(spells.back(), SPELL);
		logTrace("Created spell");

		//bullet create
		btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.z);
		m_BulletNormalSpell.emplace_back(
			m_bp->createObject(obj, 1.0f, spellPos+(directionVector*2), glm::vec3(0.3f, 0.3f, 0.3f)));
			
		int size = m_BulletNormalSpell.size();
		m_BulletNormalSpell.at(size - 1)->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		m_BulletNormalSpell.at(size - 1)->setUserPointer(spell);
		m_BulletNormalSpell.at(size - 1)->setLinearVelocity(direction*spell->getSpellBase()->m_speed);
	}

	if (type == ENHANCEATTACK)
	{
		auto spell = new AttackSpell(spellPos, directionVector, enhanceAtkBase);
		spell->setUniqueID(getUniqueID());
		Client::getInstance()->createSpellOnNetwork(*spell);
		spells.emplace_back(spell);
		Renderer::getInstance()->submit(spells.back(), SPELL);
		logTrace("Created spell");

		//bullet create
		btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.z);
		m_BulletNormalSpell.emplace_back(
			m_bp->createObject(obj, 1.0f, spellPos + directionVector * 2, glm::vec3(1.0f, 0.0f, 0.0f)));

		int size = m_BulletNormalSpell.size();
		m_BulletNormalSpell.at(size - 1)->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		m_BulletNormalSpell.at(size - 1)->setUserPointer(spell);
		m_BulletNormalSpell.at(size - 1)->setLinearVelocity(direction * spell->getSpellBase()->m_speed);
	}


	
	


	//if (type == FLAMESTRIKE)
	//{
	//	if (m_tempFlamestrike->getCooldown() <= 0)
	//	{
	//		AOEAttack tempSpell2 = *m_tempFlamestrike;
	//		tempSpell2.createSpell(deltaTime, spellPos, directionVector);
	//		m_flamestrike.push_back(tempSpell2);
	//		m_tempFlamestrike->setCooldown(7.0f);
	//
	//		m_tempSpell->setCooldown(1.0f);
	//		setType(NORMALATTACK);
	//		spellIsOver = true;
	//	}
	//}
	//return spellIsOver;
}

void SpellHandler::spellUpdate(float deltaTime)
{

	for (int i = 0; i < spells.size(); i++)
	{
		spells[i]->update(deltaTime);
		spells[i]->updateRigidbody(deltaTime, m_BulletNormalSpell.at(i));

		Client::getInstance()->updateSpellOnNetwork(*spells[i]);
		
		if (spells[i]->getTravelTime() <= 0)
		{
			logTrace("Deleted spell");
			Renderer::getInstance()->removeDynamic(spells[i], SPELL);

			Client::getInstance()->destroySpellOnNetwork(*spells[i]);
			delete spells[i];
			spells.erase(spells.begin() + i);
			m_BulletNormalSpell.erase(m_BulletNormalSpell.begin() + i);
			m_bp->removeObject(m_nrOfOtherrigidBodys);
			
		}
	}
	spellCollisionCheck();

	//for (int i = 0; i < m_flamestrike.size(); i++)
	//{
	//
	//	m_flamestrike[i].updateActiveSpell(deltaTime);
	//	
	//	std::cout << m_flamestrike[i].getSpellPos().x << " " << m_flamestrike[i].getSpellPos().y << " " << m_flamestrike[i].getSpellPos().z << std::endl;
	//	tempFire->setWorldPosition(m_flamestrike[i].getSpellPos());
	//	
	//	if (m_flamestrike[i].isAOE())
	//	{
	//		tempFire->translate(m_flamestrike[i].getSpellPos());
	//	}
	//	
	//
	//	if (m_flamestrike[i].getTravelTime() <= 0)
	//	{
	//		m_flamestrike.erase(m_flamestrike.begin() + i);
	//	}
	//}
}

const AttackSpellBase& SpellHandler::getSpellBase(SPELL_TYPE spelltype)
{
	if (spelltype == NORMALATTACK)
		return *attackBase;
	//else if(spelltype == ENHANCEATTACK)
		//return *
}


void SpellHandler::renderSpell()
{
	ShaderMap::getInstance()->useByName(BASIC_FORWARD);
	Renderer::getInstance()->renderSpell(attackBase); //Why is object null??
}

const uint64_t SpellHandler::getUniqueID()
{
	// Starts at 1 because 0 is a "Undefined" id
	static uint64_t id = 1;
	
	if (id == UINT64_MAX)
		id = 1;
	
	return id++;
}

void SpellHandler::spellCollisionCheck()
{
	//get the list of att the players on the network
	auto& list = Client::getInstance()->getNetworkPlayersREF().getPlayersREF();

	for (size_t i = 0; i < list.size(); i++)
	{
		glm::vec3 playerPos = list[i].data.position;
		list[i].data.rotation;

		//create the axis and rotate them
		glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
		std::vector<glm::vec3> axis;

		glm::rotateX(xAxis, list[i].data.rotation.x);
		glm::rotateY(xAxis, list[i].data.rotation.y);
		glm::rotateZ(xAxis, list[i].data.rotation.z);

		axis.emplace_back(xAxis);
		axis.emplace_back(yAxis);
		axis.emplace_back(zAxis);
		
		//create a box, obb or AABB? from the player position
		for (size_t j = 0; j < spells.size(); j++) {
			glm::vec3 spellPos = spells.at(j)->getTransform().position;
			float scale = spells.at(j)->getTransform().scale.x;
			if (specificSpellCollision(spellPos, playerPos, axis, scale))
			{
				Client::getInstance()->sendHitRequest(*spells[j], list[i]);
				spells[j]->setTravelTime(0.0f);
			}
		}
	}
}

bool SpellHandler::specificSpellCollision(glm::vec3 spellPos, glm::vec3 playerPos, std::vector<glm::vec3>& axis, float scale)
{ 
	// scale might be wrong, the 0.6 too
	bool collision = false;
	float sphereRadius = 0.6f * scale * 2;

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
	float boxSize = 0.5f;
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




