#include "Pch/Pch.h"
#include "SpellHandler.h"
#include <Networking/Client.h>
#include <Loader/BGLoader.h>


SpellHandler::SpellHandler()
{
	initAttackSpell();
	initEnhanceSpell();
	initFlamestrikeSpell();
	initFireSpell();
	
	if(Client::getInstance()->isSpectating() == false)
		setCharacter(CHARACTER);
}

void SpellHandler::initAttackSpell()
{
	attackBase.m_mesh = new Mesh();
	attackBase.m_material = new Material();

	// Mesh--
	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + "attackSpell.mesh");
	attackBase.m_mesh->saveFilePath(tempLoader.GetFileName(), 0);
	attackBase.m_mesh->nameMesh(tempLoader.GetMeshName());
	attackBase.m_mesh->setUpMesh(tempLoader.GetVertices(), tempLoader.GetFaces());
	attackBase.m_mesh->setUpBuffers();

	// Material--
	const Material& newMaterial = tempLoader.GetMaterial();
	attackBase.m_material->ambient = newMaterial.ambient;
	attackBase.m_material->diffuse = newMaterial.diffuse;
	attackBase.m_material->name = newMaterial.name;
	attackBase.m_material->specular = newMaterial.specular;
	attackBase.m_material->diffuse = glm::vec3(0.65f, 1.0f, 1.0f); // Light blue
	attackBase.m_material->ambient = glm::vec3(0.65f, 1.0f, 1.0f);

	// Gameplay--
	attackBase.m_lowDamage = 20.0f;
	attackBase.m_highDamage = 30.0f;
	attackBase.m_speed = 40.0f;
	attackBase.m_acceleration = 40.0f;
	attackBase.m_radius = 0.25f;
	attackBase.m_lifeTime = 5.0f;
	attackBase.m_maxBounces = 3.0f;

	// Light--
	attackBase.m_attenAndRadius = glm::vec4(1.0f, 0.14f, 0.07f, 22.0f);// OLD
	attackBase.m_attenAndRadius = glm::vec4(1.0f, 2.15f, 4.5f, 22.0f);

	PSinfo tempPS;
	TextureInfo tempTxt;
	tempTxt.name = "Assets/Textures/dots.png";
	tempPS.width = 0.4f;
	tempPS.heigth = 0.6f;
	tempPS.lifetime = 1.5f;
	tempPS.maxParticles = 1000;
	tempPS.emission = 0.002f;
	tempPS.force = -0.2f;
	tempPS.drag = 0.0f;
	tempPS.gravity = 0.0f;
	tempPS.seed = 0;
	tempPS.cont = true;
	tempPS.omnious = false;
	tempPS.spread = 0.0f;
	tempPS.glow = 2;
	tempPS.scaleDirection = 0;
	tempPS.swirl = 0;
	tempPS.fade = 1;
	tempPS.color = glm::vec3(0.0f, 0.9f, 0.9f);
	tempPS.blendColor = glm::vec3(0.8f, 1.0f, 1.0f);
	tempPS.direction = glm::vec3(1.0f, 0.0f, 0.0f);

	attackBase.m_particleBuffers.emplace_back(new ParticleBuffers(tempPS, tempTxt));
	attackBase.m_particleBuffers.back()->setShader(ShaderMap::getInstance()->getShader(PARTICLES)->getShaderID());
	attackBase.m_particleBuffers.back()->bindBuffers();
}

void SpellHandler::initEnhanceSpell()
{
	enhanceAtkBase.m_mesh = new Mesh();
	enhanceAtkBase.m_material = new Material();

	// Mesh--
	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + "enhanceSpell.mesh");
	enhanceAtkBase.m_mesh->saveFilePath(tempLoader.GetFileName(), 0);
	enhanceAtkBase.m_mesh->nameMesh(tempLoader.GetMeshName());
	enhanceAtkBase.m_mesh->setUpMesh(tempLoader.GetVertices(), tempLoader.GetFaces());
	enhanceAtkBase.m_mesh->setUpBuffers();

	// Material--
	const Material& newMaterial = tempLoader.GetMaterial();
	enhanceAtkBase.m_material->ambient = newMaterial.ambient;
	enhanceAtkBase.m_material->diffuse = newMaterial.diffuse;
	enhanceAtkBase.m_material->name = newMaterial.name;
	enhanceAtkBase.m_material->specular = newMaterial.specular;
	enhanceAtkBase.m_material->diffuse = glm::vec3(0.5f, 0.0f, 0.6f);
	enhanceAtkBase.m_material->ambient = glm::vec3(0.5f, 0.0f, 0.6f);

	// Gameplay--
	enhanceAtkBase.m_lowDamage = 10.0f;
	enhanceAtkBase.m_highDamage = 90.0f;
	enhanceAtkBase.m_speed = 70.0f;
	enhanceAtkBase.m_radius = 0.5f;
	enhanceAtkBase.m_lifeTime = 5.0f;
	enhanceAtkBase.m_maxBounces = 3.0;

	// Light--
	enhanceAtkBase.m_attenAndRadius = glm::vec4(1.0f, 0.14f, 0.07f, 22.0f); // OLD
	enhanceAtkBase.m_attenAndRadius = glm::vec4(1.0f, 1.55f, 3.7f, 22.0f);

	PSinfo tempPS;
	TextureInfo tempTxt;
	tempTxt.name = "Assets/Textures/betterStar.png";
	tempPS.width = 0.3f;
	tempPS.heigth = 0.3f;
	tempPS.lifetime = 0.3f;
	tempPS.maxParticles = 1000;
	tempPS.emission = 0.001f;
	tempPS.force = -0.2f;
	tempPS.drag = 0.0f;
	tempPS.gravity = 0.0f;
	tempPS.seed = 0;
	tempPS.cont = true;
	tempPS.omnious = false;
	tempPS.spread = -1.0f;
	tempPS.glow = 1.3;
	tempPS.scaleDirection = 0;
	tempPS.swirl = 0;
	tempPS.fade = 1;
	tempPS.color = glm::vec3(0.5f, 1.0f, 0.0f);
	tempPS.blendColor = glm::vec3(1.0f, 0.0f, 1.0f);
	tempPS.color = glm::vec3(0.0, 0.0f, 0.0f);
	tempPS.direction = glm::vec3(1.0f, 0.0f, 0.0f);

	enhanceAtkBase.m_particleBuffers.emplace_back(new ParticleBuffers(tempPS, tempTxt));
	enhanceAtkBase.m_particleBuffers.back()->setShader(ShaderMap::getInstance()->getShader(PARTICLES)->getShaderID());
	enhanceAtkBase.m_particleBuffers.back()->bindBuffers();
}

void SpellHandler::initFlamestrikeSpell()
{
	flamestrikeBase.m_mesh = new Mesh();
	flamestrikeBase.m_material = new Material();

	// Mesh--
	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + "dragonfirepotion.mesh");
	flamestrikeBase.m_mesh->saveFilePath(tempLoader.GetFileName(), 0);
	flamestrikeBase.m_mesh->nameMesh(tempLoader.GetMeshName());
	flamestrikeBase.m_mesh->setUpMesh(tempLoader.GetVertices(), tempLoader.GetFaces());
	flamestrikeBase.m_mesh->setUpBuffers();

	// Material--
	const Material& newMaterial = tempLoader.GetMaterial();
	flamestrikeBase.m_material->ambient = newMaterial.ambient;
	flamestrikeBase.m_material->diffuse = newMaterial.diffuse;
	flamestrikeBase.m_material->diffuse = glm::vec3(1.0f, 0.5f, 0.0f);
	flamestrikeBase.m_material->ambient = glm::vec3(1.0f, 0.5f, 0.0f);
	flamestrikeBase.m_material->name = newMaterial.name;
	flamestrikeBase.m_material->specular = newMaterial.specular;

	// Gameplay--
	flamestrikeBase.m_damage = 10;
	flamestrikeBase.m_speed = 55.0f;
	flamestrikeBase.m_lifeTime = 5;
	flamestrikeBase.m_maxBounces = 2;

	// Light--
	flamestrikeBase.m_attenAndRadius = glm::vec4(1.0f, 0.14f, 0.07f, 22.0f); // Old
	flamestrikeBase.m_attenAndRadius = glm::vec4(1.0f, 0.61f, 0.74f, 22.0f);
}

void SpellHandler::initFireSpell()
{
	fireBase.m_mesh = new Mesh();
	fireBase.m_material = new Material();

	// Mesh--
	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + "TestSphere.mesh");
	fireBase.m_mesh->saveFilePath(tempLoader.GetFileName(), 0);
	fireBase.m_mesh->nameMesh(tempLoader.GetMeshName());
	fireBase.m_mesh->setUpMesh(tempLoader.GetVertices(), tempLoader.GetFaces());
	fireBase.m_mesh->setUpBuffers();

	// Material--
	const Material& newMaterial = tempLoader.GetMaterial();
	fireBase.m_material->ambient = newMaterial.ambient;
	fireBase.m_material->diffuse = newMaterial.diffuse;
	fireBase.m_material->name = newMaterial.name;
	fireBase.m_material->specular = newMaterial.specular;
	fireBase.m_material->diffuse = glm::vec3(1.0f, 0.5f, 0.0f);
	fireBase.m_material->ambient = glm::vec3(1.0f, 0.5f, 0.0f);

	// Gameplay--
	fireBase.m_damage = 30.0f;
	fireBase.m_speed = 0.0f;
	fireBase.m_lifeTime = 5.0f;
	fireBase.m_maxBounces = 0.0f;

	// Light--
	fireBase.m_attenAndRadius = glm::vec4(1.0f, 0.14f, 0.07f, 22.0f); // Old
	fireBase.m_attenAndRadius = glm::vec4(1.0f, 0.61f, 0.74f, 22.0f);

	PSinfo tempPS;
	TextureInfo tempTxt;

	tempTxt.name = "Assets/Textures/betterSmoke2.png";
	tempPS.width = 0.9f;
	tempPS.heigth = 1.2f;
	tempPS.lifetime = 5.0f;
	tempPS.maxParticles = 300;
	tempPS.emission = 0.02f;
	tempPS.force = -0.54f;
	tempPS.drag = 0.0f;
	tempPS.gravity = -2.2f;
	tempPS.seed = 1;
	tempPS.cont = true;
	tempPS.omnious = true;
	tempPS.spread = 3.0f;
	tempPS.glow = 1.3;
	tempPS.scaleDirection = 0;
	tempPS.swirl = 0;
	tempPS.fade = 1;
	tempPS.randomSpawn = true;
	tempPS.color = glm::vec3(0.3f, 0.3f, 0.3f);
	tempPS.blendColor = glm::vec3(1.0f, 1.0f, 1.0f);
	tempPS.color = glm::vec3(0.0, 0.0f, 0.0f);
	tempPS.direction = glm::vec3(0.0f, -1.0f, 0.0f);

	fireBase.m_particleBuffers.emplace_back(new ParticleBuffers(tempPS, tempTxt));
	fireBase.m_particleBuffers.back()->setShader(ShaderMap::getInstance()->getShader(PARTICLES)->getShaderID());
	fireBase.m_particleBuffers.back()->bindBuffers();

	//---------------

	tempTxt.name = "Assets/Textures/Spell_2.png";
	tempPS.width = 1.2f;
	tempPS.heigth = 1.0f;
	tempPS.lifetime = 10.0f;
	tempPS.maxParticles = 1000; //350     
	tempPS.emission = 0.01f; //0.00001f;     
	tempPS.force = -0.04f; //5     
	tempPS.drag = 0.0f;
	tempPS.gravity = -0.2f; //Standard is 1     
	tempPS.seed = 1;
	tempPS.cont = true;
	tempPS.omnious = true;
	tempPS.spread = 5.0f;
	tempPS.glow = 1.3;
	tempPS.scaleDirection = 0;
	tempPS.swirl = 1;
	tempPS.fade = 1;
	tempPS.color = glm::vec3(1.0f, 0.2f, 0.0f);
	tempPS.blendColor = glm::vec3(1.0f, 1.0f, 0.1f);
	tempPS.randomSpawn = true;
	tempPS.direction = glm::vec3(0.0f, 1.0f, 0.0f);
	tempPS.direction = glm::clamp(tempPS.direction, -1.0f, 1.0f);

	fireBase.m_particleBuffers.emplace_back(new ParticleBuffers(tempPS, tempTxt));
	fireBase.m_particleBuffers.back()->setShader(ShaderMap::getInstance()->getShader(PARTICLES)->getShaderID());
	fireBase.m_particleBuffers.back()->bindBuffers();
}

SpellHandler::~SpellHandler()
{
	for (Spell* element : spells)
		delete element;
	for (Spell* element : flamestrikeSpells)
		delete element;
	for (Spell* element : fireSpells)
		delete element;

}

OBJECT_TYPE SpellHandler::createSpell(glm::vec3 spellPos, glm::vec3 directionVector, OBJECT_TYPE type)
{
	SoundHandler* shPtr = SoundHandler::getInstance();
	auto* clientPtr = Client::getInstance();

	if (type == NORMALATTACK)
	{
		// Generic
		spells.emplace_back(new AttackSpell(spellPos, directionVector, &attackBase));
		auto spell = spells.back();
		spell->setType(NORMALATTACK);

		for (int i = 0; i < attackBase.m_particleBuffers.size(); i++)
		{
			spell->addParticle(attackBase.m_particleBuffers[i]);
		}

		// Network
		spell->setUniqueID(getUniqueID());
		Client::getInstance()->createSpellOnNetwork(*spell);
		Renderer::getInstance()->submit(spell, SPELL);

		// Bullet
		spell->createRigidBody(
			BulletPhysics::getInstance()->createObject(
				sphere,
				10.0f,
				spellPos + directionVector * 2,
				glm::vec3(spell->getObjectTransform().scale.x, 0.0f, 0.0f)
			));
			
		btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.z);
		spell->getRigidBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		spell->getRigidBody()->setUserPointer(spell);
		spell->getRigidBody()->setLinearVelocity(direction * attackBase.m_speed);
		spell->getRigidBody()->setDamping(0.0f, 0.0f);
	}

	if (type == ENHANCEATTACK)
	{
		// Generic
		spells.emplace_back(new AttackSpell(spellPos, directionVector, &enhanceAtkBase));
		auto spell = spells.back();
		spell->setType(ENHANCEATTACK);

		for (int i = 0; i < enhanceAtkBase.m_particleBuffers.size(); i++)
		{
			spell->addParticle(enhanceAtkBase.m_particleBuffers[i]);
		}

		// Network
		spell->setUniqueID(getUniqueID());
		Client::getInstance()->createSpellOnNetwork(*spell);
		Renderer::getInstance()->submit(spell, SPELL);
		
		// Bullet
		spell->createRigidBody(
			BulletPhysics::getInstance()->createObject(
				sphere,
				30.0f,
				spellPos + directionVector * 2,
				glm::vec3(spell->getObjectTransform().scale.x, 0.0f, 0.0f)
			));

		btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.z);
		spell->getRigidBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		spell->getRigidBody()->setUserPointer(spell);
		spell->getRigidBody()->setLinearVelocity(direction * enhanceAtkBase.m_speed);

		// Sound
		int slot = shPtr->playSound(EnhanceAttackSound, clientPtr->getMyData().guid);	

		
	}

	if (type == FLAMESTRIKE)
	{
		// Generic
		flamestrikeSpells.emplace_back(new AOEAttack(spellPos, directionVector, &flamestrikeBase));
		auto spell = flamestrikeSpells.back();
		Renderer::getInstance()->submit(flamestrikeSpells.back(), SPELL);

		// Network
		spell->setUniqueID(getUniqueID());
		Client::getInstance()->createSpellOnNetwork(*spell);

		// Bullet
		btVector3 direction = btVector3(directionVector.x, directionVector.y, directionVector.z);
		spell->createRigidBody(BulletPhysics::getInstance()->createObject(
			sphere,
			1.0f,
			spellPos + directionVector * 2,
			glm::vec3(spell->getObjectTransform().scale.x, 0.0f, 0.0f),
			glm::quat(),
			false,
			0.15f,
			1.0f
		));

		spell->getRigidBody()->setGravity(btVector3(0.0f, -60.0f, 0.0f));
		float rndX = rand() % 2000 + 1 - 1000; rndX /= 100;
		float rndY = rand() % 2000 + 1 - 1000; rndY /= 100;
		float rndZ = rand() % 2000 + 1 - 1000; rndZ /= 100;
		spell->getRigidBody()->setAngularVelocity(btVector3(rndX, rndY, rndZ));
		spell->getRigidBody()->setLinearVelocity(btVector3(direction * flamestrikeBase.m_speed));
		spell->getRigidBody()->setUserPointer(spell);

		// Sound
		spell->setSoundSlot(shPtr->playSound(FireSound, clientPtr->getMyData().guid));
		if (spell->getSoundSlot() != -1) //out of slots
		{
			shPtr->setSourcePosition(spell->getObjectTransform().position, FireSound, clientPtr->getMyData().guid, spell->getSoundSlot());
		}		
	}

	if (type == FIRE)
	{
		// Generic
		fireSpells.emplace_back(new fire(spellPos, directionVector, &fireBase));
		auto spell = fireSpells.back();
		Renderer::getInstance()->submit(fireSpells.back(), SPELL);

		for (int i = 0; i < fireBase.m_particleBuffers.size(); i++)
		{
			spell->addParticle(fireBase.m_particleBuffers[i]);
		}

		// Network
		spell->setUniqueID(getUniqueID());
		Client::getInstance()->createSpellOnNetwork(*spell);

		// Sound
		shPtr->setSourcePosition(spellPos, GlassBreakSound, clientPtr->getMyData().guid);
		shPtr->playSound(GlassBreakSound, clientPtr->getMyData().guid);	


	}

	// Randomize spell
	srand(NULL);
	int rndType = type;
	int tries = 0;
	while (rndType == type && tries != 50)
	{
		rndType = NORMALATTACK + (rand() % (FLAMESTRIKE - NORMALATTACK) + 1);
		if (tries == 50)
			rndType = NORMALATTACK;
	}
	OBJECT_TYPE randomSpell = (OBJECT_TYPE)rndType;

	return randomSpell;
}

void SpellHandler::spellUpdate(float deltaTime)
{
	for (size_t i = 0; i < flamestrikeSpells.size(); i++)
	{		
		if (flamestrikeSpells[i]->getTravelTime() > 0)
		{			
			flamestrikeSpells[i]->update(deltaTime);
			
			if (flamestrikeSpells[i]->getType() == FLAMESTRIKE)
			{				
				AOEAttack* flamestrike = static_cast<AOEAttack*>(flamestrikeSpells[i]);
				
				if (flamestrikeSpells[i]->getSoundSlot() != -1)
				{
					SoundHandler::getInstance()->setSourcePosition(flamestrike->getObjectTransform().position,
						FireSound, Client::getInstance()->getMyData().guid,
						flamestrikeSpells[i]->getSoundSlot());	
				}

				if (flamestrike->spellOnGround())
				{
					createSpell(flamestrike->getObjectTransform().position, glm::vec3(0, 0, 0), FIRE);
					flamestrike->setSpellBool(false);
				}
			}

			Client::getInstance()->updateSpellOnNetwork(*flamestrikeSpells[i]);
		}
		if (flamestrikeSpells[i]->getTravelTime() <= 0)
		{
			Renderer::getInstance()->removeRenderObject(flamestrikeSpells[i], SPELL);

			Client::getInstance()->destroySpellOnNetwork(*flamestrikeSpells[i]);
			delete flamestrikeSpells[i];
			flamestrikeSpells.erase(flamestrikeSpells.begin() + i);
		}
	}

	for (size_t i = 0; i < fireSpells.size(); i++)
	{
		if (fireSpells[i]->getTravelTime() > 0)
		{
			fireSpells[i]->update(deltaTime);

			Client::getInstance()->updateSpellOnNetwork(*fireSpells[i]);
			fireSpells[i]->UpdateParticles(deltaTime);
		}

		if (fireSpells[i]->getTravelTime() <= 0)
		{
			Renderer::getInstance()->removeRenderObject(fireSpells[i], SPELL);

			Client::getInstance()->destroySpellOnNetwork(*fireSpells[i]);
			delete fireSpells[i];
			fireSpells.erase(fireSpells.begin() + i);
		}
	}


	for (size_t i = 0; i < spells.size(); i++)
	{
		if (spells[i]->getTravelTime() > 0)
		{
			spells[i]->update(deltaTime);
			Client::getInstance()->updateSpellOnNetwork(*spells[i]);
			spells[i]->UpdateParticles(deltaTime);
		}

		if (spells[i]->getTravelTime() <= 0)
		{
			Renderer::getInstance()->removeRenderObject(spells[i], SPELL);

			Client::getInstance()->destroySpellOnNetwork(*spells[i]);
			delete spells[i];
			spells.erase(spells.begin() + i);
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


const SpellBase* SpellHandler::getSpellBase(OBJECT_TYPE type) const
{
	switch (type)
	{
	case (NORMALATTACK):
		return &attackBase;
	case (ENHANCEATTACK):
		return &enhanceAtkBase;
	case (FLAMESTRIKE):
		return &flamestrikeBase;
	case (FIRE):
		return &fireBase;
	}

	return nullptr;
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

	if (ownPlayer.health > 0 && list.size() >= 0)
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

		
		//Me and fire spell
		for (size_t j = 0; j < fireSpells.size(); j++)
		{
			glm::vec3 spellPos = fireSpells.at(j)->getObjectTransform().position;

			float scale = fireSpells.at(j)->getObjectTransform().scale.x;
			
			if (specificSpellCollision(spellPos, ownPlayerPos, axis1, scale))
			{
				if (ownfireDamageCounter <= 0)
				{
					Client::getInstance()->sendHitRequest(*fireSpells[j], ownPlayer);
					ownfireDamageCounter = 0.5f;
				}
			}
		}	
	}

	//move camera and spell collision
	for (size_t i = 0; i < list.size() && (1 <= spells.size() || 1<= fireSpells.size()); i++)
	{
		if (list[i].data.health <= 0 || list[i].data.invulnerabilityTime > 0.0f)
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
			glm::vec3 spellPos = spells.at(j)->getObjectTransform().position;

			//get the radius from the spelltype
			float radius = 0.0f;

			if (spells[j]->getType() == NORMALATTACK) {
				radius = attackBase.m_radius;
			}
			if (spells[j]->getType() == ENHANCEATTACK) {
				radius = enhanceAtkBase.m_radius;
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

		for (size_t j = 0; j < fireSpells.size(); j++)
		{
			glm::vec3 spellPos = fireSpells.at(j)->getObjectTransform().position;

			float scale = fireSpells.at(j)->getObjectTransform().scale.x;

			if (specificSpellCollision(spellPos, playerPos, axis, scale))
			{
				if (fireDamageCounter <= 0)
				{
					Client::getInstance()->sendHitRequest(*fireSpells[j], list[i]);
					fireDamageCounter = 0.5f;
					
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
	btVector3 box = BulletPhysics::getInstance()->getCharacterSize();
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
	//glm::vec3 halfSize = glm::vec3((max - min) * 0.5f) * 0.7f;
	halfSize.x *= 0.6f;
	halfSize.z *= 0.6f;

	BulletPhysics::getInstance()->setCharacterSize(halfSize);
	m_setcharacter = true;
}



