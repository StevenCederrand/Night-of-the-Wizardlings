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

	const Material& newMaterial = tempLoader.GetMaterial();
	attackBase->m_material->ambient = newMaterial.ambient;
	attackBase->m_material->diffuse = newMaterial.diffuse;
	attackBase->m_material->name = newMaterial.name;
	attackBase->m_material->specular = newMaterial.specular;
	tempLoader.Unload();

	attackBase->m_damage = 34;
	attackBase->m_speed = 10;
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

void SpellHandler::createSpell(glm::vec3 spellPos, glm::vec3 directionVector, TYPE type)
{

	if (type == NORMALATTACK)
	{
		spells.emplace_back(new AttackSpell(spellPos, directionVector, attackBase));
		logTrace("Created spell");
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

		if (spells[i]->getTravelTime() <= 0)
		{
			delete spells[i];
			spells.erase(spells.begin() + i);
			logTrace("Deleted spell");
		}
	}
}