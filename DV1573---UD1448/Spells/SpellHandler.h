#pragma once
#include <Pch/Pch.h>
#include <Spells/Spells.h>
#include <GameObject/GameObject.h>
#include <Spells/AttackSpell.h>
#include <Spells/EnhanceAttackSpell.h>
#include <Spells/ReflectSpell.h>
#include <Spells/AOEAttack.h>
#include <System/BulletPhysics.h>
#include "SpellTypes.h"


//enum TYPE { NORMALATTACK, ENHANCEATTACK };
class Client;

class SpellHandler
{
public:
	SpellHandler(BulletPhysics* bp);
	void initAttackSpell();
	void initEnhanceSpell();
	void initFlamestrikeSpell();
	void initReflectSpell();
	~SpellHandler();

	float createSpell(glm::vec3 spellPos, glm::vec3 directionVector, SPELL_TYPE type);

	void spellUpdate(float deltaTime);
	void setSpawnerPosition(glm::vec3 position);
	void setSpawnerDirection(glm::vec3 direction);
	void setOnHitCallback(std::function<void()> func);

	const Spell& getSpell(int index) const { return *spells[index]; }
	const std::vector<Spell*>& getSpells() const { return spells; }

	const AttackSpellBase* getAttackBase() { return attackBase; }
	const AttackSpellBase* getEnhAttackBase() { return enhanceAtkBase; }
	const ReflectSpellBase* getReflectBase() { return reflectBase; }
	const FlamestrikeSpellBase* getFlamestrikeBase() { return flamestrikeBase; }

	void renderSpell();


private:
	const uint64_t getUniqueID();
	bool m_newHit = true;
	bool m_setcharacter = false;
	float m_nrSubSteps = 6;

	std::vector<Spell*> spells;
	glm::vec3 m_spawnerPos;
	glm::vec3 m_spawnerDir;

	// The base for all basic attack spells
	AttackSpellBase* attackBase;
	AttackSpellBase* enhanceAtkBase;
	ReflectSpellBase* reflectBase;
	FlamestrikeSpellBase* flamestrikeBase;

	void spellCollisionCheck();	
	bool specificSpellCollision(glm::vec3 spellPos, glm::vec3 playerPos, std::vector<glm::vec3>& axis, float radius);
	//glm::vec3 OBBclosestPoint(glm::vec3 &spherePos, std::vector<glm::vec3> &axis, glm::vec3 &playerPos);
	float OBBsqDist(glm::vec3& spherePos, std::vector<glm::vec3>& axis, glm::vec3& playerPos);

	void REFLECTupdate(float deltaTime, int i);
	void flamestrikeUpdate(float deltaTime, int i);
	

	BulletPhysics* m_bp;
	std::vector<btRigidBody*> m_BulletNormalSpell;
	std::vector<btRigidBody*> m_BulletEnhanceAttackSpell;
	std::vector<btRigidBody*> m_BulletFlamestrikeSpell;
	
	// Don't touch if you don't know what you are doing
	friend class Client;

	struct deflectSpellData {
		glm::vec3 direction;
		glm::vec3 position;
		SPELL_TYPE type;
	};

	std::vector<deflectSpellData> m_deflectedSpells;
	std::function<void()> m_onHitCallback;
};