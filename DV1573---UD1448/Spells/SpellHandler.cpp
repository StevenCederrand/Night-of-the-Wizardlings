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
	fireBase = nullptr;
	reflectBase = nullptr;

	initAttackSpell();
	initEnhanceSpell();
	initFlamestrikeSpell();
	initReflectSpell();
	initFireSpell();
	
	if(bp != nullptr && Client::getInstance()->isSpectating() == false)
		setCharacter(CHARACTER);
}

void SpellHandler::initAttackSpell()
{
	attackBase = new AttackSpellBase();
	attackBase->m_mesh = new Mesh();
	attackBase->m_material = new Material();

	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + "attackSpell.mesh");
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

	attackBase->m_damage = 34.0f;
	attackBase->m_speed = 70.0f;
	attackBase->m_radius = 0.25f;
	attackBase->m_coolDown = 1.0f;
	attackBase->m_lifeTime = 5.0f;
	attackBase->m_maxBounces = 3.0f;
}

void SpellHandler::initEnhanceSpell()
{
	enhanceAtkBase = new AttackSpellBase();
	enhanceAtkBase->m_mesh = new Mesh();
	enhanceAtkBase->m_material = new Material();

	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + "enhanceSpell.mesh");
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

	//enhanceAtkBase->m_material->diffuse = glm::vec3(0.3f, 1.0f, 0.3f);
	//enhanceAtkBase->m_material->ambient = glm::vec3(0.3f, 1.0f, 0.3f);
	enhanceAtkBase->m_material->diffuse = glm::vec3(0.85f, 0.3f, 0.2f);
	enhanceAtkBase->m_material->ambient = glm::vec3(0.85f, 0.3f, 0.2f);

	enhanceAtkBase->m_damage = 34.0f;
	enhanceAtkBase->m_speed = 150.0f;
	enhanceAtkBase->m_radius = 0.5f;
	enhanceAtkBase->m_coolDown = 3.0f;
	enhanceAtkBase->m_lifeTime = 5.0f;
	enhanceAtkBase->m_maxBounces = 3;
}

void SpellHandler::initFlamestrikeSpell()
{
	flamestrikeBase = new FlamestrikeSpellBase();
	flamestrikeBase->m_mesh = new Mesh();
	flamestrikeBase->m_material = new Material();

	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + "dragonfirepotion.mesh");
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

	flamestrikeBase->m_material->diffuse = glm::vec3(1.0f, 0.5f, 0.0f);
	flamestrikeBase->m_material->ambient = glm::vec3(1.0f, 0.5f, 0.0f);

	flamestrikeBase->m_damage = 10;
	flamestrikeBase->m_speed = 55.0f;
	flamestrikeBase->m_coolDown = 1;
	flamestrikeBase->m_lifeTime = 5;
	flamestrikeBase->m_maxBounces = 2;
}

void SpellHandler::initFireSpell()
{
	fireBase = new FireSpellBase();
	fireBase->m_mesh = new Mesh();
	fireBase->m_material = new Material();

	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + "TestSphere.mesh");
	fireBase->m_mesh->saveFilePath(tempLoader.GetFileName(), 0);
	fireBase->m_mesh->nameMesh(tempLoader.GetMeshName());
	fireBase->m_mesh->setUpMesh(tempLoader.GetVertices(), tempLoader.GetFaces());
	fireBase->m_mesh->setUpBuffers();

	const Material & newMaterial = tempLoader.GetMaterial();
	fireBase->m_material->ambient = newMaterial.ambient;
	fireBase->m_material->diffuse = newMaterial.diffuse;
	fireBase->m_material->name = newMaterial.name;
	fireBase->m_material->specular = newMaterial.specular;
	tempLoader.Unload();

	fireBase->m_material->diffuse = glm::vec3(1.0f, 0.5f, 0.0f);
	fireBase->m_material->ambient = glm::vec3(1.0f, 0.5f, 0.0f);

	fireBase->m_damage = 10.0f;
	fireBase->m_speed = 0.0f;
	fireBase->m_coolDown = 4.0f;
	fireBase->m_lifeTime = 5.0f;
	fireBase->m_maxBounces = 0;
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
	if (fireBase)
		delete fireBase;

	for (Spell* element : spells)
		delete element;
	spells.clear();

	for (Spell* element : flamestrikeSpells)
		delete element;
	flamestrikeSpells.clear();

	for (Spell* element : fireSpells)
		delete element;
	fireSpells.clear();
}

float SpellHandler::createSpell(glm::vec3 spellPos, glm::vec3 directionVector, OBJECT_TYPE type)
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
			m_bp->createObject(sphere, 10.0f, spellPos+directionVector*2, glm::vec3(spell->getTransform().scale.x, 0.0f, 0.0f)));
			
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
			m_bp->createObject(sphere, 30.0f, spellPos + directionVector * 2, glm::vec3(spell->getTransform().scale.x, 0.0f, 0.0f)));

		int size = m_BulletNormalSpell.size();
		m_BulletNormalSpell.at(size - 1)->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		m_BulletNormalSpell.at(size - 1)->setUserPointer(spell);
		m_BulletNormalSpell.at(size - 1)->setLinearVelocity(direction * enhanceAtkBase->m_speed);
	}

	if (type == REFLECT)
	{
		auto spell = new ReflectSpell(spellPos, directionVector, reflectBase);
		cooldown = reflectBase->m_coolDown;
		spell->setUniqueID(getUniqueID());
		Client::getInstance()->createSpellOnNetwork(*spell);
		spells.emplace_back(spell);
		Renderer::getInstance()->submit(spells.back(), SPELL);
		logTrace("Created reflect spell");

		//bullet create
		btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.x);
		m_BulletNormalSpell.emplace_back(
			m_bp->createObject(sphere, 1.0f, spellPos + directionVector * 2, glm::vec3(spell->getTransform().scale.x, 0.0f, 0.0f)));

		int size = m_BulletNormalSpell.size();
		m_BulletNormalSpell.at(size - 1)->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		m_BulletNormalSpell.at(size - 1)->setUserPointer(m_BulletNormalSpell.at(size - 1));
	}

	if (type == FLAMESTRIKE)
	{
		auto spell = new AOEAttack(spellPos, directionVector, flamestrikeBase);
		cooldown = flamestrikeBase->m_coolDown;

		spell->setUniqueID(getUniqueID());
		Client::getInstance()->createSpellOnNetwork(*spell);
		flamestrikeSpells.emplace_back(spell);
		Renderer::getInstance()->submit(flamestrikeSpells.back(), SPELL);

		//bullet create
		btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.z);
		m_BulletFlamestrikeSpell.emplace_back(m_bp->createObject(
			sphere,
			1.0f,
			spellPos + directionVector * 2,
			glm::vec3(spell->getTransform().scale.x, 0.0f, 0.0f),
			glm::quat(),
			false,
			0.15f,
			1.0f
		));

		int size = m_BulletFlamestrikeSpell.size();
		m_BulletFlamestrikeSpell.back()->setGravity(btVector3(0.0f, -60.0f, 0.0f));
		float rndX = rand() % 1999 + 1 - 1000; rndX /= 100;
		float rndY = rand() % 1999 + 1 - 1000; rndY /= 100;
		float rndZ = rand() % 1999 + 1 - 1000; rndZ /= 100;
		m_BulletFlamestrikeSpell.back()->setAngularVelocity(btVector3(rndX, rndY, rndZ));
		m_BulletFlamestrikeSpell.back()->setLinearVelocity(btVector3(direction * flamestrikeBase->m_speed));
		m_BulletFlamestrikeSpell.back()->setUserPointer(spell);
	}

	if (type == FIRE)
	{
		auto fireSpell = new fire(spellPos, directionVector, fireBase);
		cooldown = fireBase->m_coolDown;

		fireSpell->setUniqueID(getUniqueID());
		Client::getInstance()->createSpellOnNetwork(*fireSpell);
		fireSpells.emplace_back(fireSpell);
		Renderer::getInstance()->submit(fireSpells.back(), SPELL);
		
	}

	return cooldown;
}

void SpellHandler::spellUpdate(float deltaTime)
{
	for (size_t i = 0; i < flamestrikeSpells.size(); i++)
	{
		if (flamestrikeSpells[i]->getTravelTime() > 0)
		{
			flamestrikeSpells[i]->update(deltaTime);
			flamestrikeSpells[i]->updateRigidbody(deltaTime, m_BulletFlamestrikeSpell.at(i));
			
			if (static_cast<Spell*>(flamestrikeSpells[i])->getType() == FLAMESTRIKE)
			{
				flamestrikeUpdate(deltaTime, i);
				AOEAttack* flamestrike = static_cast<AOEAttack*>(flamestrikeSpells[i]);
				//flamestrike->updateActiveSpell(deltaTime);
				if (flamestrike->spellOnGround())
				{
					createSpell(flamestrike->getTransform().position, glm::vec3(0, 0, 0), FIRE);
					flamestrike->setSpellBool(false);
				}
			}
			


			Client::getInstance()->updateSpellOnNetwork(*flamestrikeSpells[i]);
		}
		if (flamestrikeSpells[i]->getTravelTime() <= 0)
		{
			Renderer::getInstance()->removeDynamic(flamestrikeSpells[i], SPELL);

			Client::getInstance()->destroySpellOnNetwork(*flamestrikeSpells[i]);
			delete flamestrikeSpells[i];
			flamestrikeSpells.erase(flamestrikeSpells.begin() + i);

			m_bp->removeObject(m_BulletFlamestrikeSpell.at(i));
			m_BulletFlamestrikeSpell.erase(m_BulletFlamestrikeSpell.begin() + i);
		}

	}

	for (size_t i = 0; i < fireSpells.size(); i++)
	{
		if (fireSpells[i]->getTravelTime() > 0)
		{
			fireSpells[i]->update(deltaTime);

			Client::getInstance()->updateSpellOnNetwork(*fireSpells[i]);


		}

		if (fireSpells[i]->getTravelTime() <= 0)
		{
			Renderer::getInstance()->removeDynamic(fireSpells[i], SPELL);

			Client::getInstance()->destroySpellOnNetwork(*fireSpells[i]);
			delete fireSpells[i];
			fireSpells.erase(fireSpells.begin() + i);
		}
	}

	if (Input::isKeyReleased(GLFW_KEY_L))
	{
		m_newHit = !m_newHit;
	}

	for (size_t i = 0; i < spells.size(); i++)
	{
		if (spells[i]->getTravelTime() > 0)
		{
			
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
		std::lock_guard<std::mutex> lockGuard(NetGlobals::UpdateDeflectSpellMutex);
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

	auto list = Client::getInstance()->getNetworkPlayersREF().getPlayersREF();
	auto ownPlayer = Client::getInstance()->getMyData();
	
	fireDamageCounter = fireDamageCounter - DeltaTime * 1;
	ownfireDamageCounter = ownfireDamageCounter - DeltaTime * 1;

	if (ownPlayer.health > 0 && list.size() >= 1)
	{
		glm::vec3 ownPlayerPos = ownPlayer.position;

		glm::vec3 xAxis1 = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 yAxis1 = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 zAxis1 = glm::vec3(0.0f, 0.0f, 1.0f);
		std::vector<glm::vec3> axis1;

		glm::rotateX(xAxis1, ownPlayer.rotation.x);
		glm::rotateY(yAxis1, ownPlayer.rotation.y);
		glm::rotateZ(zAxis1, ownPlayer.rotation.z);

		axis1.emplace_back(xAxis1);
		axis1.emplace_back(yAxis1);
		axis1.emplace_back(zAxis1);

		if (!m_newHit)
		{
			//Me and fire spell
			for (size_t j = 0; j < fireSpells.size(); j++)
			{
				glm::vec3 spellPos = fireSpells.at(j)->getTransform().position;

				float scale = fireSpells.at(j)->getTransform().scale.x;
			
				if (specificSpellCollision(spellPos, ownPlayerPos, axis1, scale))
				{
					if (ownfireDamageCounter <= 0)
					{
						Client::getInstance()->sendHitRequest(*fireSpells[j], ownPlayer);
						ownfireDamageCounter = 1.0f;
					}
				}
			}
		}
	}
	//move camera and spell collision
	for (size_t i = 0; i < list.size() && 1 <= spells.size() ; i++)
	{
		if (list[i].data.health <= 0)
			continue;

		glm::vec3 playerPos = list[i].data.position;
		
		//create the axis and rotate them
		glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
		std::vector<glm::vec3> axis;

		glm::quat playerrot = glm::quat(list[i].data.rotation);
		xAxis =	glm::rotate(playerrot, xAxis);
		yAxis = glm::rotate(playerrot, yAxis);
		zAxis = glm::rotate(playerrot, zAxis);

		axis.emplace_back(xAxis);
		axis.emplace_back(yAxis);
		axis.emplace_back(zAxis);

		//create a box, obb or AABB? from the player position. Old hitdetection press L
		for (size_t j = 0; j < spells.size(); j++)
		{
			glm::vec3 lastSpellPos = spells.at(j)->getLastPosition();
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

					if (m_onHitCallback != nullptr) {
						m_onHitCallback();
					}
					k = static_cast<size_t>(m_nrSubSteps);
				}
			}
		}

		for (size_t k = 0; k < fireSpells.size(); k++)
		{
			glm::vec3 lastSpellPos = fireSpells.at(k)->getLastPosition();
			glm::vec3 spellPos = fireSpells.at(k)->getTransform().position;

			//get the radius from the spelltype
			float radius = fireBase->m_radius;

			//line is the walking we will do.
			glm::vec3 line = (spellPos - lastSpellPos) / m_nrSubSteps;
			glm::vec3 interpolationPos = lastSpellPos;

			//walk from last pos to new pos with substeps
			for (size_t l = 0; l < m_nrSubSteps; l++)
			{
				interpolationPos += line;
				if (specificSpellCollision(interpolationPos, playerPos, axis, radius))
				{
					Client::getInstance()->sendHitRequest(*spells[k], list[i]);

					if (m_onHitCallback != nullptr) {
						m_onHitCallback();
					}
					k = m_nrSubSteps;
				}
			}
		}
	}
}

bool SpellHandler::specificSpellCollision(glm::vec3 spellPos, glm::vec3 playerPos, std::vector<glm::vec3>& axis, float radius)
{ 
	bool collision = false;
	float sphereRadius = 1.0f * radius;

	float distx2 = OBBsqDist(spellPos, axis, playerPos);

	if (distx2 <= sphereRadius * sphereRadius)
	{
		collision = true;
	}
	return collision;
}

float SpellHandler::OBBsqDist(glm::vec3& spherePos, std::vector<glm::vec3>& axis, glm::vec3& playerPos)
{	
	btVector3 box = m_bp->getCharacterSize();
	glm::vec3 halfSize = glm::vec3(box.getX(), box.getY(), box.getZ());
	
	float dist = 0.0f;
	//closest point on obb
	glm::vec3 boxPoint = playerPos;
	boxPoint.y += halfSize.y;

	glm::vec3 ray = glm::vec3(spherePos - boxPoint);

	for (int j = 0; j < 3; j++) {
		float distance = glm::dot(ray, axis.at(j));
		float distance2 = 0;

		if (distance > halfSize[j])
			distance2 = distance - halfSize[j];

		if (distance < -halfSize[j])
			distance2 = distance + halfSize[j];

		dist += distance2 * distance2;
	}
	return dist;
}

void SpellHandler::setCharacter(std::string meshName)
{
	std::string skeleton = MeshMap::getInstance()->getMesh(meshName)->getSkeleton();
	glm::vec3 min;
	glm::vec3 max;

	if (skeleton[0] == NULL)
	{
		const std::vector<Vertex> vertices = MeshMap::getInstance()->getMesh(meshName)->getVertices();
		min = vertices[0].position;
		max = vertices[0].position;

		for (size_t i = 1; i < vertices.size(); i++)
		{
			min.x = fminf(vertices[i].position.x, min.x);
			min.y = fminf(vertices[i].position.y, min.y);
			min.z = fminf(vertices[i].position.z, min.z);

			max.x = fmaxf(vertices[i].position.x, max.x);
			max.y = fmaxf(vertices[i].position.y, max.y);
			max.z = fmaxf(vertices[i].position.z, max.z);
		}
	}
	else
	{
		const std::vector<Vertex2> vertices = MeshMap::getInstance()->getMesh(meshName)->getVerticesSkele();
		min = vertices[0].position;
		max = vertices[0].position;

		for (size_t i = 1; i < vertices.size(); i++)
		{
			min.x = fminf(vertices[i].position.x, min.x);
			min.y = fminf(vertices[i].position.y, min.y);
			min.z = fminf(vertices[i].position.z, min.z);

			max.x = fmaxf(vertices[i].position.x, max.x);
			max.y = fmaxf(vertices[i].position.y, max.y);
			max.z = fmaxf(vertices[i].position.z, max.z);
		}
	}
	glm::vec3 halfSize = glm::vec3((max - min) * 0.5f);

	m_bp->setCharacterSize(halfSize);
	m_setcharacter = true;
}

void SpellHandler::REFLECTupdate(float deltaTime, int i)
{
	ReflectSpell* reflectSpell = static_cast<ReflectSpell*>(spells[i]);
	reflectSpell->updateReflection(deltaTime, m_BulletNormalSpell.at(i), m_spawnerPos, m_spawnerDir);

	auto spellList = Client::getInstance()->getNetworkSpells();
	for (size_t i = 0; i < spellList.size(); i++)
	{
		float hitboxRadius = 0.0f;
		OBJECT_TYPE type = spellList[i].SpellType;
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
	AOEAttack* flamestrike = static_cast<AOEAttack*>(flamestrikeSpells[i]);
	flamestrike->updateActiveSpell(deltaTime);
}