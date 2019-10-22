#include "Pch/Pch.h"
#include "SpellHandler.h"
#include <Networking/Client.h>
#include <Loader/BGLoader.h>


SpellHandler::SpellHandler(BulletPhysics * bp)
{
	m_bp = bp;

	attackBase = nullptr;
	enhanceAtkBase = nullptr;
	flamestrikeBase = nullptr;
	reflectBase = nullptr;

	initAttackSpell();
	initEnhanceSpell();
	initFlamestrikeSpell();
	initReflectSpell();
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
	attackBase->m_speed = 45;
	attackBase->m_radius = 0.2;
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
	tempLoader.LoadMesh(MESHPATH + "TestCube.mesh");
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

	attackBase->m_material->diffuse = glm::vec3(0.65f, 0.7f, 1.0f);
	attackBase->m_material->ambient = glm::vec3(0.65f, 0.7f, 1.0f);

	enhanceAtkBase->m_damage = 34.0f;
	enhanceAtkBase->m_speed = 75.0f;
	enhanceAtkBase->m_radius = 0.2f;
	enhanceAtkBase->m_coolDown = 1.0f;
	enhanceAtkBase->m_lifeTime = 5.0f;
	enhanceAtkBase->m_maxBounces = 3.0f;
}

void SpellHandler::initFlamestrikeSpell()
{
	flamestrikeBase = new FlamestrikeSpellBase();
	flamestrikeBase->m_mesh = new Mesh();
	flamestrikeBase->m_material = new Material();

	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + "TestSphere.mesh");
	flamestrikeBase->m_mesh->saveFilePath(tempLoader.GetFileName(), 0);
	flamestrikeBase->m_mesh->nameMesh(tempLoader.GetMeshName());
	flamestrikeBase->m_mesh->setUpMesh(tempLoader.GetVertices(), tempLoader.GetFaces());
	flamestrikeBase->m_mesh->setUpBuffers();

	const Material & newMaterial = tempLoader.GetMaterial();
	flamestrikeBase->m_material->ambient = newMaterial.ambient;
	flamestrikeBase->m_material->diffuse = newMaterial.diffuse;
	flamestrikeBase->m_material->name = newMaterial.name;
	flamestrikeBase->m_material->specular = newMaterial.specular;
	tempLoader.Unload();

	flamestrikeBase->m_material->diffuse = glm::vec3(0.65f, 1.0f, 1.0f);
	flamestrikeBase->m_material->ambient = glm::vec3(0.65f, 1.0f, 1.0f);

	flamestrikeBase->m_damage = 10;
	flamestrikeBase->m_speed = 25;
	flamestrikeBase->m_coolDown = 1;
	flamestrikeBase->m_lifeTime = 5;
	flamestrikeBase->m_maxBounces = 3;
}

void SpellHandler::initReflectSpell()
{
	reflectBase = new ReflectSpellBase();
	reflectBase->m_mesh = new Mesh();
	reflectBase->m_material = new Material();

	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + "TestSphere.mesh");
	reflectBase->m_mesh->saveFilePath(tempLoader.GetFileName(), 0);
	reflectBase->m_mesh->nameMesh(tempLoader.GetMeshName());
	reflectBase->m_mesh->setUpMesh(tempLoader.GetVertices(), tempLoader.GetFaces());
	reflectBase->m_mesh->setUpBuffers();

	const Material& newMaterial = tempLoader.GetMaterial();
	reflectBase->m_material->ambient = newMaterial.ambient;
	reflectBase->m_material->diffuse = newMaterial.diffuse;
	reflectBase->m_material->name = newMaterial.name;
	reflectBase->m_material->specular = newMaterial.specular;
	tempLoader.Unload();

	reflectBase->m_material->diffuse = glm::vec3(1.0f, 0.0f, 0.5f);
	reflectBase->m_material->ambient = glm::vec3(1.0f, 0.0f, 0.5f);

	reflectBase->m_radius = 1.0f;
	reflectBase->m_coolDown = 2.0f;
	reflectBase->m_lifeTime = 10.0f;
}

SpellHandler::~SpellHandler()
{
	if (attackBase)
		delete attackBase;
	if (enhanceAtkBase)
		delete enhanceAtkBase;
	if (flamestrikeBase)
		delete flamestrikeBase;
	if (reflectBase)
		delete reflectBase;


	for (Spell* element : spells)
		delete element;

	spells.clear();
}

float SpellHandler::createSpell(glm::vec3 spellPos, glm::vec3 directionVector, SPELL_TYPE type)
{
	float cooldown = 0.0f;
	CollisionObject obj = sphere;
	if (type == NORMALATTACK)
	{
		auto spell = new AttackSpell(spellPos, directionVector, attackBase);
		cooldown = attackBase->m_coolDown;

		spell->setUniqueID(getUniqueID());
		Client::getInstance()->createSpellOnNetwork(*spell);
		spells.emplace_back(spell);
		Renderer::getInstance()->submit(spells.back(), SPELL);
		logTrace("Created attack spell");

		//bullet create
		btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.x);
		m_BulletNormalSpell.emplace_back(
			m_bp->createObject(obj, 1.0f, spellPos+directionVector*2, glm::vec3(spell->getTransform().scale.x, 0.0f, 0.0f)));
			
		int size = m_BulletNormalSpell.size();
		m_BulletNormalSpell.at(size - 1)->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		m_BulletNormalSpell.at(size - 1)->setUserPointer(m_BulletNormalSpell.at(size - 1));
	}

	if (type == ENHANCEATTACK)
	{
		auto spell = new AttackSpell(spellPos, directionVector, enhanceAtkBase);
		cooldown = enhanceAtkBase->m_coolDown;

		spell->setUniqueID(getUniqueID());
		Client::getInstance()->createSpellOnNetwork(*spell);
		spells.emplace_back(spell);
		Renderer::getInstance()->submit(spells.back(), SPELL);
		logTrace("Created enhanceattack spell");

		//bullet create
		btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.x);
		m_BulletNormalSpell.emplace_back(
			m_bp->createObject(obj, 1.0f, spellPos + directionVector * 2, glm::vec3(spell->getTransform().scale.x, 0.0f, 0.0f)));

		int size = m_BulletNormalSpell.size();
		m_BulletNormalSpell.at(size - 1)->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		m_BulletNormalSpell.at(size - 1)->setUserPointer(m_BulletNormalSpell.at(size - 1));
	}

	if (type == REFLECT)
	{
		auto spell = new ReflectSpell(spellPos, directionVector, reflectBase);
		cooldown = reflectBase->m_coolDown;
		//Client::getInstance()->createSpellOnNetwork(*spell);
		spell->setUniqueID(getUniqueID());
		spells.emplace_back(spell);
		Renderer::getInstance()->submit(spells.back(), SPELL);
		logTrace("Created reflect spell");

		//bullet create
		btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.x);
		m_BulletNormalSpell.emplace_back(
			m_bp->createObject(obj, 1.0f, spellPos + directionVector * 2, glm::vec3(spell->getTransform().scale.x, 0.0f, 0.0f)));

		int size = m_BulletNormalSpell.size();
		m_BulletNormalSpell.at(size - 1)->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		m_BulletNormalSpell.at(size - 1)->setUserPointer(m_BulletNormalSpell.at(size - 1));
	}


	return cooldown;
}

void SpellHandler::spellUpdate(float deltaTime)
{
	for (int i = 0; i < spells.size(); i++)
	{
		spells[i]->update(deltaTime);
		spells[i]->updateRigidbody(deltaTime, m_BulletNormalSpell.at(i));

		if (static_cast<Spell*>(spells[i])->getType() == REFLECT)
		{
			REFLECTupdate(deltaTime, i);
		}


		Client::getInstance()->updateSpellOnNetwork(*spells[i]);
		
		if (spells[i]->getTravelTime() <= 0)
		{
			logTrace("Deleted spell");
			Renderer::getInstance()->removeDynamic(spells[i], SPELL);

			Client::getInstance()->destroySpellOnNetwork(*spells[i]);
			delete spells[i];
			spells.erase(spells.begin() + i);
			m_BulletNormalSpell.erase(m_BulletNormalSpell.begin() + i);
		}
	}
	
	spellCollisionCheck();
}

void SpellHandler::setSpawnerPosition(glm::vec3 position)
{
	m_spawnerPos = position;
}

void SpellHandler::setSpawnerDirection(glm::vec3 direction)
{
	m_spawnerDir = direction;
}

void SpellHandler::renderSpell()
{
	ShaderMap::getInstance()->useByName(BASIC_FORWARD);
	Renderer::getInstance()->renderSpell();
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
			if (specificSpellCollision(spellPos, playerPos, axis))
			{
				Client::getInstance()->sendHitRequest(*spells[j], list[i]);
				spells[j]->setTravelTime(0.0f);
			}
		}
	}

	
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

void SpellHandler::REFLECTupdate(float deltaTime, int i)
{
	ReflectSpell* reflectSpell = static_cast<ReflectSpell*>(spells[i]);
	reflectSpell->updateReflection(deltaTime, m_BulletNormalSpell.at(i), m_spawnerPos, m_spawnerDir);

	auto& spellList = Client::getInstance()->getNetworkSpells();
	for (size_t i = 0; i < spellList.size(); i++)
	{
		float hitboxRadius = 0.0f;
		SPELL_TYPE type = spellList[i].SpellType;
		switch (type)
		{
		case NORMALATTACK:
			hitboxRadius = attackBase->m_radius;
			break;
		case ENHANCEATTACK:
			hitboxRadius = enhanceAtkBase->m_radius;
			break;
		case REFLECT:
			hitboxRadius = reflectBase->m_radius;
			break;
		default:
			break;
		}

		if (reflectSpell->checkReflectCollision(spellList[i].Position, spellList[i].Direction, hitboxRadius))
		{
			//TODO: Delete incoming spell
			//Client::getInstance()->destroySpellOnNetwork(*spells[i]);
			createSpell(m_spawnerPos, m_spawnerDir, spellList[i].SpellType);
			logTrace("Collision with reflection");
		}
	}
}






