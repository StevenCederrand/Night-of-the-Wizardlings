#include "Pch/Pch.h"
#include "SpellHandler.h"
#include <Networking/Client.h>
#include <Loader/BGLoader.h>


SpellHandler::SpellHandler(BulletPhysics * bp)
{
	attackBase = nullptr;
	initAttackSpell();
	m_bp = bp;
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

	attackBase->m_damage = 34;
	attackBase->m_speed = 25;
	attackBase->m_coolDown = 1;
	attackBase->m_lifeTime = 5;
	attackBase->m_maxBounces = 3;
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
		btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.x);
		m_BulletNormalSpell.emplace_back(
			m_bp->createObject(obj, 1.0f, spellPos+directionVector*2, glm::vec3(1.0f, 0.0f, 0.0f)));
			
		int size = m_BulletNormalSpell.size();
		m_BulletNormalSpell.at(size - 1)->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		m_BulletNormalSpell.at(size - 1)->setUserPointer(m_BulletNormalSpell.at(size - 1));
	}

	if (type == ENHANCEATTACK)
	{
		//spells.emplace_back(new EnhanceAttackSpell(spellPos, directionVector));
	}	
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
			Client::getInstance()->destroySpellOnNetwork(*spells[i]);
			delete spells[i];
			spells.erase(spells.begin() + i);
			logTrace("Deleted spell");
			m_BulletNormalSpell.erase(m_BulletNormalSpell.begin() + i);
		}
	}
	spellCollisionCheck();

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
	for (int i = 0; i < spells.size(); i++) {

		glm::vec3 spellPos = spells.at(i)->getTransform().position;
		//glm::vec3 spellPos = spells.at(i).getTransform().position;
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
		for (int i = 0; i < spells.size(); i++) {
			glm::vec3 spherePos = spells.at(i)->getTransform().position;
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
