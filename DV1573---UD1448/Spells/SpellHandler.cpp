#include "Pch/Pch.h"
#include "SpellHandler.h"
#include <Loader/BGLoader.h>

SpellHandler::SpellHandler()
{
	attackBase = nullptr;
	initAttackSpell();
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


void SpellHandler::initEnhanceSpell()
{
	enhanceHandlerBase = new EnhanceHanderSpellBase();
	enhanceHandlerBase->m_attackCooldown = 0.3f;
	enhanceHandlerBase->m_nrOfAttacks = 3;

	enhanceatkBase = new EnhanceAtkSpellBase();
	enhanceatkBase->m_material = new Material();

	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + "TestSphere.mesh");

	enhanceatkBase->m_mesh = new Mesh();
	enhanceatkBase->m_mesh->saveFilePath(tempLoader.GetFileName(), 0);
	enhanceatkBase->m_mesh->nameMesh(tempLoader.GetMeshName());
	enhanceatkBase->m_mesh->setUpMesh(tempLoader.GetVertices(), tempLoader.GetFaces());
	enhanceatkBase->m_mesh->setUpBuffers();

	const Material& newMaterial = tempLoader.GetMaterial();
	enhanceatkBase->m_material->ambient = newMaterial.ambient;
	enhanceatkBase->m_material->diffuse = newMaterial.diffuse;
	enhanceatkBase->m_material->name = newMaterial.name;
	enhanceatkBase->m_material->specular = newMaterial.specular;
	tempLoader.Unload();

	enhanceatkBase->m_damage = 34;
	enhanceatkBase->m_speed = 50;
	enhanceatkBase->m_coolDown = 1;
	enhanceatkBase->m_lifeTime = 5;
	enhanceatkBase->m_maxBounces = 3;
}




SpellHandler::~SpellHandler()
{
	if (attackBase)
		delete attackBase;
	for (Spell* element : spells)
		delete element;
	spells.clear();
}



void SpellHandler::createSpell(glm::vec3 spellPos, glm::vec3 directionVector, SPELLTYPE type)
{

	if (type == NORMALATTACK)
	{
		spells.emplace_back(new AttackSpell(spellPos, directionVector, attackBase));
		Renderer::getInstance()->submit(spells.back(), SPELL);
		logTrace("Created spell");
	}

	if (type == ENHANCEHANDLER)
	{
		spells.emplace_back(new EnhanceAttackSpell(enhanceHandlerBase));
		logTrace("Created spell");
	}

	if (type == ENHANCEATTACK)
	{
		//spells.emplace_back(new EnhanceAttackSpell(spellPos, directionVector, enhanceatkBase));
		//Renderer::getInstance()->submit(spells.back(), SPELL);
		//logTrace("Created spell");
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
		if (spells[i]->getTravelTime() <= 0)
		{
			delete spells[i];
			spells.erase(spells.begin() + i);
			logTrace("Deleted spell");
		}
	}

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

const AttackSpellBase& SpellHandler::getSpellBase(SPELLTYPE spelltype)
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
	
	//for (EnhanceAttackSpell object : enhanceAttackSpell)
	//{
	//	object.bindMaterialToShader("Basic_Forward");
	//	Renderer::getInstance()->renderSpell(object);
	//}
	
	//for (AttackSpell object : m_normalSpell)
	//{
	//	object.bindMaterialToShader("Basic_Forward");
	//	Renderer::getInstance()->render(object);
	//}
	//
	//for (EnhanceAttackSpell object : m_enhanceAttackSpell)
	//{
	//	object.bindMaterialToShader("Basic_Forward");
	//	Renderer::getInstance()->render(object);
	//}
	//
	//for (AOEAttack object : m_flamestrike)
	//{
	//	object.bindMaterialToShader("Basic_Forward");
	//	Renderer::getInstance()->render(object);
	//}
	//for (int i = 0; i < m_flamestrike.size(); i++)
	//{
	//	if (m_flamestrike[i].isAOE())
	//	{
	//		tempFire->bindMaterialToShader("Basic_Forward");
	//		Renderer::getInstance()->render(*tempFire);
	//	}
	//}
	
}




