#pragma once
#include <Pch/Pch.h>
#include <Spells/Spells.h>
#include <GameObject/GameObject.h>
#include <Spells/AttackSpell.h>
#include <Spells/EnhanceAttackSpell.h>
#include <Spells/AOEAttack.h>
#include <Spells/fire.h>
#include <System/BulletPhysics.h>
#include <GameObject/ObjectTypes.h>


//enum TYPE { NORMALATTACK, ENHANCEATTACK };
class Client;

class SpellHandler
{
public:
	SpellHandler();
	~SpellHandler();

	OBJECT_TYPE createSpell(glm::vec3 spellPos, glm::vec3 directionVector, OBJECT_TYPE type);

	void spellUpdate(float deltaTime);
	void setSpawnerPosition(glm::vec3 position);
	void setSpawnerDirection(glm::vec3 direction);
	void setOnHitCallback(std::function<void()> func);

	const Spell& getSpell(int index) const { return *spells[index]; }
	const std::vector<Spell*>& getSpells() const { return spells; }
	const SpellBase* getSpellBase(OBJECT_TYPE type) const;

	void renderSpell();

private:
	void initAttackSpell();
	void initEnhanceSpell();
	void initFlamestrikeSpell();
	void initFireSpell();

	const uint64_t getUniqueID();
	bool m_setcharacter = false;
	float m_nrSubSteps = 6;

	std::vector<Spell*> spells;
	std::vector<Spell*> flamestrikeSpells;
	std::vector<Spell*> fireSpells;
	float fireDamageCounter = 0.5f;
	float ownfireDamageCounter = 0.5f;

	glm::vec3 m_spawnerPos;
	glm::vec3 m_spawnerDir;

	// The base for all basic attack spells
	SpellBase attackBase;
	SpellBase enhanceAtkBase;
	SpellBase flamestrikeBase;
	SpellBase fireBase;
	SpellBase death;

	void spellCollisionCheck();	
	bool specificSpellCollision(glm::vec3 spellPos, glm::vec3 playerPos, std::vector<glm::vec3>& axis, float radius);
	float OBBsqDist(glm::vec3& spherePos, std::vector<glm::vec3>& axis, glm::vec3& playerPos);

	void setCharacter(std::string meshName);
	
	// Don't touch if you don't know what you are doing
	friend class Client;

	struct deflectSpellData {
		glm::vec3 direction;
		glm::vec3 position;
		OBJECT_TYPE type;
	};

	std::vector<deflectSpellData> m_deflectedSpells;
	std::function<void()> m_onHitCallback;
};