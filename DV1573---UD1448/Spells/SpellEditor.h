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

class Client;

class SpellEditor
{
public:
	SpellEditor();
	~SpellEditor();

	void createSpellForTool(glm::vec3 spellPos, glm::vec3 directionVector, OBJECT_TYPE type);
	void spellToolUpdate(float dt, float radius, float speed);
	void changeSpell(int state);

	void renderSpell();

	const Spell& getSpell(int index) const { return *spells[index]; }
	const std::vector<Spell*>& getSpells() const { return spells; }
	const uint64_t getUniqueID();
	const SpellBase* getSpellBase(OBJECT_TYPE type) const;

private:
	void initAttackSpell();
	void initEnhanceSpell();
	void initFlamestrikeSpell();
	void initFireSpell();

	std::vector<Spell*> spells;
	std::vector<Spell*> flamestrikeSpells;
	std::vector<Spell*> fireSpells;

	SpellLoader myLoader;

	// The base for all basic attack spells
	SpellBase attackBase;
	SpellBase enhanceAtkBase;
	SpellBase flamestrikeBase;
	SpellBase fireBase;

	// Don't touch if you don't know what you are doing
	friend class Client;

	//SpellEditor
	int activespell;
};