#pragma once
#include <Pch/Pch.h>
#include <Spells/Spells.h>
#include <GameObject/GameObject.h>
#include <Spells/AttackSpell.h>
#include <Spells/EnhanceAttackSpell.h>
#include <System/BulletPhysics.h>
#include "SpellTypes.h"


//enum TYPE { NORMALATTACK, ENHANCEATTACK };

class SpellHandler
{
public:
	SpellHandler(BulletPhysics* bp);
	void initAttackSpell();
	void initEnhanceSpell();


	~SpellHandler();
	void createSpell(glm::vec3 spellPos, glm::vec3 directionVector, SPELL_TYPE type);

	void spellUpdate(float deltaTime);
	void updateReflection(glm::vec3 position, glm::vec3 direction);

	const AttackSpellBase& getSpellBase(SPELL_TYPE spelltype);
	const Spell& getSpell(int index) const { return *spells[index]; }
	const std::vector<Spell*>& getSpells() const { return spells; }
	void renderSpell();

	//bool isSpellReadyToCast(SPELLTYPE type);


private:
	const uint64_t getUniqueID();

private:

	std::vector<Spell*> spells;

	// The base for all basic attack spells
	AttackSpellBase* attackBase;
	AttackSpellBase* enhanceAtkBase;

	void spellCollisionCheck();
	bool specificSpellCollision(glm::vec3 spellPos, glm::vec3 playerPos, std::vector<glm::vec3>& axis);
	glm::vec3 OBBclosestPoint(glm::vec3 &spherePos, std::vector<glm::vec3> &axis, glm::vec3 &playerPos);
	
	void reflectCollisionCheck();

	BulletPhysics* m_bp;
	std::vector<btRigidBody*> m_BulletNormalSpell;
	std::vector<btRigidBody*> m_BulletEnhanceAttackSpell;
};