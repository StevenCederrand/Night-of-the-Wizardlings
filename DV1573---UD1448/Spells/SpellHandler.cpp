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
	attackBase->m_speed = 110;
	attackBase->m_radius = 0.5;
	attackBase->m_coolDown = 3;
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

	enhanceAtkBase->m_material->diffuse = glm::vec3(0.85f, 0.3f, 0.2f);
	enhanceAtkBase->m_material->ambient = glm::vec3(0.85f, 0.3f, 0.2f);

	enhanceAtkBase->m_damage = 34.0f;
	enhanceAtkBase->m_speed = 120.0f;
	enhanceAtkBase->m_radius = 0.5f;
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

	flamestrikeBase->m_material->diffuse = glm::vec3(1.0f, 0.0f, 0.5f);
	flamestrikeBase->m_material->ambient = glm::vec3(1.0f, 0.0f, 0.5f);

	flamestrikeBase->m_damage = 10;
	flamestrikeBase->m_speed = 50;
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
	reflectBase->m_coolDown = 10.0f;
	reflectBase->m_lifeTime = 2.5f;
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
	if (Client::getInstance()->getMyData().health <= 0)
		return cooldown;
	
	if (type == NORMALATTACK)
	{
		auto spell = new AttackSpell(spellPos, directionVector, attackBase);
		spell->setType(NORMALATTACK);
		cooldown = attackBase->m_coolDown;

		spell->setUniqueID(getUniqueID());
		Client::getInstance()->createSpellOnNetwork(*spell);
		spells.emplace_back(spell);
		Renderer::getInstance()->submit(spells.back(), SPELL);

		//bullet create
		btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.z);
		m_BulletNormalSpell.emplace_back(
			m_bp->createObject(sphere, 1.0f, spellPos+directionVector*2, glm::vec3(spell->getTransform().scale.x, 0.0f, 0.0f)));
			
		int size = m_BulletNormalSpell.size();
		m_BulletNormalSpell.at(size - 1)->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		m_BulletNormalSpell.at(size - 1)->setUserPointer(spell);
		m_BulletNormalSpell.at(size - 1)->setLinearVelocity(direction * attackBase->m_speed);
	}

	if (type == ENHANCEATTACK)
	{
		auto spell = new AttackSpell(spellPos, directionVector, enhanceAtkBase);
		spell->setType(ENHANCEATTACK);
		cooldown = enhanceAtkBase->m_coolDown;

		spell->setUniqueID(getUniqueID());
		Client::getInstance()->createSpellOnNetwork(*spell);
		spells.emplace_back(spell);
		Renderer::getInstance()->submit(spells.back(), SPELL);
		
		//bullet create
		btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.z);
		m_BulletNormalSpell.emplace_back(
			m_bp->createObject(sphere, 1.0f, spellPos + directionVector * 2, glm::vec3(spell->getTransform().scale.x, 0.0f, 0.0f)));

		int size = m_BulletNormalSpell.size();
		m_BulletNormalSpell.at(size - 1)->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		m_BulletNormalSpell.at(size - 1)->setUserPointer(spell);
		m_BulletNormalSpell.at(size - 1)->setLinearVelocity(direction * enhanceAtkBase->m_speed);
	}

	//if (type == REFLECT)
	//{
	//	auto spell = new ReflectSpell(spellPos, directionVector, reflectBase);
	//	cooldown = reflectBase->m_coolDown;
	//	spell->setUniqueID(getUniqueID());
	//	Client::getInstance()->createSpellOnNetwork(*spell);
	//	spells.emplace_back(spell);
	//	Renderer::getInstance()->submit(spells.back(), SPELL);
	//	logTrace("Created reflect spell");

	//	//bullet create
	//	btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.x);
	//	m_BulletNormalSpell.emplace_back(
	//		m_bp->createObject(sphere, 1.0f, spellPos + directionVector * 2, glm::vec3(spell->getTransform().scale.x, 0.0f, 0.0f)));

	//	int size = m_BulletNormalSpell.size();
	//	m_BulletNormalSpell.at(size - 1)->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	//	m_BulletNormalSpell.at(size - 1)->setUserPointer(m_BulletNormalSpell.at(size - 1));
	//}

	//if (type == FLAMESTRIKE)
	//{
	//	auto spell = new AOEAttack(spellPos, directionVector, flamestrikeBase);
	//	cooldown = flamestrikeBase->m_coolDown;

	//	spell->setUniqueID(getUniqueID());
	//	Client::getInstance()->createSpellOnNetwork(*spell);
	//	spells.emplace_back(spell);
	//	Renderer::getInstance()->submit(spells.back(), SPELL);
	//	logTrace("Created flamestrike spell");

	//	//bullet create
	//	btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.x);
	//	m_BulletNormalSpell.emplace_back(
	//		m_bp->createObject(sphere, 1.0f, spellPos + directionVector * 2, glm::vec3(spell->getTransform().scale.x, 0.0f, 0.0f)));

	//	int size = m_BulletNormalSpell.size();
	//	m_BulletNormalSpell.at(size - 1)->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	//	m_BulletNormalSpell.at(size - 1)->setUserPointer(m_BulletNormalSpell.at(size - 1));
	//}

	return cooldown;
}

void SpellHandler::spellUpdate(float deltaTime)
{
	if (Input::isKeyReleased(GLFW_KEY_L))
	{
		m_newHit = !m_newHit;
	}
	for (size_t i = 0; i < spells.size(); i++)
	{
		if (spells[i]->getTravelTime() > 0)
		{
			/*if (static_cast<Spell*>(spells[i])->getType() == FLAMESTRIKE)
			{
				flamestrikeUpdate(deltaTime, i);
			}
			*/
			spells[i]->update(deltaTime);
			spells[i]->updateRigidbody(deltaTime, m_BulletNormalSpell.at(i));
			Client::getInstance()->updateSpellOnNetwork(*spells[i]);
		}

		if (spells[i]->getTravelTime() <= 0)
		{
			Renderer::getInstance()->removeDynamic(spells[i], SPELL);

			Client::getInstance()->destroySpellOnNetwork(*spells[i]);
			delete spells[i];
			spells.erase(spells.begin() + i);

			m_bp->removeObject(m_BulletNormalSpell.at(i));
			m_BulletNormalSpell.erase(m_BulletNormalSpell.begin() + i);
		}
	}
	spellCollisionCheck();
	
	// Scope
	{
		std::lock_guard<std::mutex> guard(m_clientSyncMutex);
		for (size_t i = 0; i < m_deflectedSpells.size(); i++)
		{
			deflectSpellData& data = m_deflectedSpells[i];
			createSpell(data.position, data.direction, data.type);
		}
	}
	m_deflectedSpells.clear();
}

void SpellHandler::setSpawnerPosition(glm::vec3 position)
{
	m_spawnerPos = position;
}

void SpellHandler::setSpawnerDirection(glm::vec3 direction)
{
	m_spawnerDir = direction;
}

void SpellHandler::setOnHitCallback(std::function<void()> func)
{
	m_onHitCallback = func;
}

void SpellHandler::renderSpell()
{
	ShaderMap::getInstance()->useByName(BASIC_FORWARD);
	Renderer::getInstance()->renderSpell(this);
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
		if (list[i].data.health <= 0)
			continue;

		glm::vec3 playerPos = list[i].data.position;

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
		
		//create a box, obb or AABB? from the player position. Old hitdetection press L
		if (!m_newHit)
		{
			for (size_t j = 0; j < spells.size(); j++) {
				logTrace("Old hit test");
				glm::vec3 spellPos = spells.at(j)->getTransform().position;
				float scale = spells.at(j)->getTransform().scale.x * 4.0f; //tested
				if (specificSpellCollision(spellPos, playerPos, axis, scale))
				{
					spells[j]->setTravelTime(0.0f);
					Client::getInstance()->sendHitRequest(*spells[j], list[i]);

					if (m_onHitCallback != nullptr) {
						m_onHitCallback();
					}
				}
			}
		}
		else
		{
			for (size_t j = 0; j < spells.size(); j++)
			{
				glm::vec3 lastSpellPos = spells.at(j)->getlastPosition();
				glm::vec3 spellPos = spells.at(j)->getTransform().position;

				//get the radius from the spelltype
				float radius = 0.0;
				if (static_cast<Spell*>(spells[i])->getType() == NORMALATTACK) {
					radius = attackBase->m_radius;
				}

				if (static_cast<Spell*>(spells[i])->getType() == ENHANCEATTACK) {
					radius = enhanceAtkBase->m_radius;
				}

				//line is the walking we will do.
				glm::vec3 line = (spellPos - lastSpellPos) / m_nrSubSteps;
				glm::vec3 interpolationPos = lastSpellPos;

				//walk from last pos to new pos with substeps
				for (size_t k = 0; k < m_nrSubSteps; k++)
				{
					interpolationPos += line;
					if (specificSpellCollision(interpolationPos, playerPos, axis, radius))
					{
						spells[j]->setTravelTime(0.0f);
						Client::getInstance()->sendHitRequest(*spells[j], list[i]);

						logTrace("Hit on substep: " + std::to_string(k));
						logTrace("Pos for player hit x: " 
							+ std::to_string(playerPos.x)
							+ " y: " + std::to_string(playerPos.y)
							+ " z: " + std::to_string(playerPos.z));

						logTrace("Pos for spell hit x: "
							+ std::to_string(interpolationPos.x)
							+ " y: " + std::to_string(interpolationPos.y)
							+ " z: " + std::to_string(interpolationPos.z));

						logTrace("OLDPos for spell x:"
							+ std::to_string(lastSpellPos.x)
							+ " y: " + std::to_string(lastSpellPos.y)
							+ " z: " + std::to_string(lastSpellPos.z));

						logTrace("Pos for the spell x:" 
							+ std::to_string(spellPos.x)
							+ " y: " + std::to_string(spellPos.y)
							+ " z: " + std::to_string(spellPos.z));



						interpolationPos = lastSpellPos;
						for (size_t l = 0; l < m_nrSubSteps; l++)
						{
							interpolationPos += line;
							logTrace("Walking spell Pos x: " + std::to_string(interpolationPos.x)
								+ " y: " + std::to_string(interpolationPos.y)
								+ " z: " + std::to_string(interpolationPos.z));

							float temp = glm::length(interpolationPos - playerPos);

							logTrace("Length of the spell: " + std::to_string(temp));
						}
						logTrace(" ");

						if (m_onHitCallback != nullptr) {
							m_onHitCallback();
						}
						k = m_nrSubSteps;
					}
				}
			}
		}
	}
}

bool SpellHandler::specificSpellCollision(glm::vec3 spellPos, glm::vec3 playerPos, std::vector<glm::vec3>& axis, float radius)
{ 
	// sphereradius is wrong
	bool collision = false;
	float sphereRadius = 1.0f * radius;

	glm::vec3 closestPoint = OBBclosestPoint(spellPos, axis, playerPos);
	glm::vec3 v = closestPoint - spellPos;

	if (glm::dot(v, v) <= sphereRadius * sphereRadius)
	{
		collision = true;
	}
	return collision;
}

glm::vec3 SpellHandler::OBBclosestPoint(glm::vec3& spherePos, std::vector<glm::vec3>& axis, glm::vec3& playerPos)
{
	btVector3 box = m_bp->getCharacterSize();
	float boxSize = box.getX();
	
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
			createSpell(m_spawnerPos, m_spawnerDir, spellList[i].SpellType);
			
		}
	}
}

void SpellHandler::flamestrikeUpdate(float deltaTime, int i)
{
	AOEAttack* flamestrike = static_cast<AOEAttack*>(spells[i]);
	flamestrike->updateActiveSpell(deltaTime);
}