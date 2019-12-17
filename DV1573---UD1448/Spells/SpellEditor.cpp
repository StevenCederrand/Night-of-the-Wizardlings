#include "Pch/Pch.h"
#include "SpellEditor.h"

SpellEditor::SpellEditor()
{
	initAttackSpell();
	initFireSpell();
	renderSpellBool = true;
	/*if (Client::getInstance()->isSpectating() == false)
		setCharacter(CHARACTER);*/
}

SpellEditor::~SpellEditor()
{
	for (Spell* element : spells)
		delete element;
	for (Spell* element : fireSpells)
		delete element;
}

void SpellEditor::initAttackSpell()
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
	const Material & newMaterial = tempLoader.GetMaterial();
	attackBase.m_material->ambient = newMaterial.ambient;
	attackBase.m_material->diffuse = newMaterial.diffuse;
	attackBase.m_material->name = newMaterial.name;
	attackBase.m_material->specular = newMaterial.specular;


	myLoader.LoadSpell("spelll.spell", NORMALATTACK);

	// Gameplay--
	attackBase.m_lowDamage		= myLoader.m_projectile.lowDamage;
	attackBase.m_highDamage		= myLoader.m_projectile.highDamage;
	attackBase.m_speed			= myLoader.m_projectile.speed;
	attackBase.m_acceleration	= 40.0f;
	attackBase.m_radius			= myLoader.m_projectile.radius;
	attackBase.m_lifeTime		= myLoader.m_projectile.lifeTime;
	attackBase.m_maxBounces		= myLoader.m_projectile.maxBounces;

	attackBase.m_material->diffuse = myLoader.m_projectile.color; // Light blue
	attackBase.m_material->ambient = myLoader.m_projectile.color;
	// Light--
	attackBase.m_attenAndRadius = glm::vec4(1.0f, 0.14f, 0.07f, 22.0f);// OLD
	attackBase.m_attenAndRadius = glm::vec4(1.0f, 2.15f, 4.5f, 22.0f);

	tempTxt.name = myLoader.m_txtInfo.name;
	tempPS.width = myLoader.m_psInfo.width;
	tempPS.heigth = myLoader.m_psInfo.heigth;
	tempPS.lifetime = myLoader.m_psInfo.lifetime;
	tempPS.maxParticles = myLoader.m_psInfo.maxParticles;
	tempPS.emission = myLoader.m_psInfo.emission;
	tempPS.force = myLoader.m_psInfo.force;
	tempPS.drag = myLoader.m_psInfo.drag;
	tempPS.gravity = myLoader.m_psInfo.gravity;
	tempPS.seed = 0;
	tempPS.cont = myLoader.m_psInfo.cont;
	tempPS.omnious = myLoader.m_psInfo.omnious;
	tempPS.randomSpawn = myLoader.m_psInfo.randomSpawn;
	tempPS.spread = myLoader.m_psInfo.spread;
	tempPS.glow = myLoader.m_psInfo.glow;
	tempPS.scaleDirection = myLoader.m_psInfo.scaleDirection;
	tempPS.swirl = myLoader.m_psInfo.swirl;
	tempPS.fade = myLoader.m_psInfo.fade;
	tempPS.color = myLoader.m_psInfo.color;
	tempPS.blendColor = myLoader.m_psInfo.blendColor;
	tempPS.direction = glm::vec3(1.0f, 0.0f, 0.0f);
	tempPS.direction = glm::clamp(tempPS.direction, -1.0f, 1.0f);

	attackBase.m_particleBuffers.emplace_back(new ParticleBuffers(tempPS, tempTxt));
	attackBase.m_particleBuffers.back()->setShader(ShaderMap::getInstance()->getShader(PARTICLES)->getShaderID());
	attackBase.m_particleBuffers.back()->bindBuffers();
}

void SpellEditor::initFireSpell()
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
	const Material & newMaterial = tempLoader.GetMaterial();
	fireBase.m_material->ambient = newMaterial.ambient;
	fireBase.m_material->diffuse = newMaterial.diffuse;
	fireBase.m_material->name = newMaterial.name;
	fireBase.m_material->specular = newMaterial.specular;
	fireBase.m_material->diffuse = glm::vec3(1.0f, 0.5f, 0.0f);
	fireBase.m_material->ambient = glm::vec3(1.0f, 0.5f, 0.0f);


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
	tempPS.randomSpawn = false;
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
	tempPS.direction = glm::clamp(tempPS.direction, -1.0f, 1.0f); //Do i need this???

	fireBase.m_particleBuffers.emplace_back(new ParticleBuffers(tempPS, tempTxt));
	fireBase.m_particleBuffers.back()->setShader(ShaderMap::getInstance()->getShader(PARTICLES)->getShaderID());
	fireBase.m_particleBuffers.back()->bindBuffers();


	myLoader.LoadSpell("spellNameFire.spell", FIRE);

	// Gameplay--
	fireBase.m_damage = myLoader.m_AOESpell.damage;
	fireBase.m_speed = myLoader.m_AOESpell.speed;
	fireBase.m_lifeTime = myLoader.m_AOESpell.lifeTime;
	fireBase.m_maxBounces = myLoader.m_AOESpell.maxBounces;

	tempTxt.name = myLoader.m_txtInfo.name;
	tempPS.width = myLoader.m_psInfo.width;
	tempPS.heigth = myLoader.m_psInfo.heigth;
	tempPS.lifetime = myLoader.m_psInfo.lifetime;
	tempPS.maxParticles = myLoader.m_psInfo.maxParticles;
	tempPS.emission = myLoader.m_psInfo.emission;
	tempPS.force = myLoader.m_psInfo.force;
	tempPS.drag = myLoader.m_psInfo.drag;
	tempPS.gravity = myLoader.m_psInfo.gravity;
	tempPS.seed = 0;
	tempPS.cont = myLoader.m_psInfo.cont;
	tempPS.omnious = myLoader.m_psInfo.omnious;
	tempPS.randomSpawn = myLoader.m_psInfo.randomSpawn;
	tempPS.spread = myLoader.m_psInfo.spread;
	tempPS.glow = myLoader.m_psInfo.glow;
	tempPS.scaleDirection = myLoader.m_psInfo.scaleDirection;
	tempPS.swirl = myLoader.m_psInfo.swirl;
	tempPS.fade = myLoader.m_psInfo.fade;
	tempPS.color = myLoader.m_psInfo.color;
	tempPS.blendColor = myLoader.m_psInfo.blendColor;
	tempPS.direction = glm::vec3(1.0f, 0.0f, 0.0f);
	tempPS.direction = glm::clamp(tempPS.direction, -1.0f, 1.0f);

	fireBase.m_particleBuffers.emplace_back(new ParticleBuffers(tempPS, tempTxt));
	fireBase.m_particleBuffers.back()->setShader(ShaderMap::getInstance()->getShader(PARTICLES)->getShaderID());
	fireBase.m_particleBuffers.back()->bindBuffers();
}

void SpellEditor::createSpellForTool(glm::vec3 spellPos, glm::vec3 directionVector, OBJECT_TYPE type)
{

	float cooldown = 0.0f;
	if (type == NORMALATTACKTOOL)
	{
		// Generic
		spells.emplace_back(new AttackSpell(spellPos, directionVector, &attackBase));
		auto spell = spells.back();
		spell->setType(NORMALATTACKTOOL);

		for (int i = 0; i < attackBase.m_particleBuffers.size(); i++)
		{
			spell->addParticle(attackBase.m_particleBuffers[i]);
		}

		// Network
		spell->setUniqueID(getUniqueID());
		Renderer::getInstance()->submit(spell, SPELL);
	}

	if (type == FIRETOOL)
	{
		// Generic

		fireSpells.emplace_back(new fire(spellPos, directionVector, &fireBase));
		auto spell = fireSpells.back();
		spell->setType(FIRETOOL);
		spell->setShouldRender(true);
		Renderer::getInstance()->submit(fireSpells.back(), SPELL);

		for (int i = 0; i < fireBase.m_particleBuffers.size(); i++)
		{
			spell->addParticle(fireBase.m_particleBuffers[i]);
		}
	}
}

void SpellEditor::spellToolUpdate(float dt, PSinfo psInfo, SpellLoading::Projectile projectileInfo, TextureInfo txtInfo, bool spellRenderer, SpellLoading::AOESpell aoeSpellInfo)
{
	for (size_t i = 0; i < spells.size(); i++)
	{
		spells[i]->updateTool(projectileInfo.radius, projectileInfo.speed, dt);
		spells[i]->UpdateParticles(dt, psInfo);
		spells[i]->UpdateTexture(txtInfo);

		attackBase.m_material->diffuse = projectileInfo.color;
		attackBase.m_material->ambient = projectileInfo.color;

		renderSpellBool = spellRenderer;

		if (activespell == 1)
		{
			spells[i]->setTravelTime(0);
		}

		if (spells[i]->getTravelTime() <= 0)
		{
			Renderer::getInstance()->removeRenderObject(spells[i], SPELL);

			Client::getInstance()->destroySpellOnNetwork(*spells[i]);
			delete spells[i];
			spells.erase(spells.begin() + i);
		}
	}

	for (size_t i = 0; i < fireSpells.size(); i++)
	{
		fireSpells[i]->updateTool(aoeSpellInfo.radius, aoeSpellInfo.speed, dt);
		fireSpells[i]->UpdateParticles(dt);
		fireSpells[i]->UpdateParticles(dt, psInfo);
		fireSpells[i]->UpdateTexture(txtInfo);
		fireSpells[i]->setShouldRender(true);

		fireBase.m_material->diffuse = projectileInfo.color;
		fireBase.m_material->ambient = projectileInfo.color;

		renderSpellBool = spellRenderer;


		if (activespell == 0)
		{
			fireSpells[i]->setTravelTime(0);
		}

		if (fireSpells[i]->getTravelTime() <= 0)
		{
			Renderer::getInstance()->removeRenderObject(fireSpells[i], SPELL);
			delete fireSpells[i];
			fireSpells.erase(fireSpells.begin() + i);
		}
	}
}

void SpellEditor::changeSpell(int state)
{
	activespell = state;
}

void SpellEditor::renderSpell()
{
	ShaderMap::getInstance()->useByName(BASIC_FORWARD);
	if(renderSpellBool == true)
		Renderer::getInstance()->renderSpell(this);
}

const uint64_t SpellEditor::getUniqueID()
{
	// Starts at 1 because 0 is a "Undefined" id
	static uint64_t id = 1;

	if (id == UINT64_MAX)
		id = 1;

	return id++;
}

const SpellBase* SpellEditor::getSpellBase(OBJECT_TYPE type) const
{
	switch (type)
	{
	case (NORMALATTACKTOOL):
		return &attackBase;
	case (ENHANCEATTACKTOOL):
		return &enhanceAtkBase;
	case (FLAMESTRIKETOOL):
		return &flamestrikeBase;
	case (FIRETOOL):
		return &fireBase;
	}

	return nullptr;
}