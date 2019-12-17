#pragma once
#include <Pch/Pch.h>

class Client;

class SpellEditor
{
public:
	SpellEditor();
	~SpellEditor();

	void createSpellForTool(glm::vec3 spellPos, glm::vec3 directionVector, OBJECT_TYPE type);
	void spellToolUpdate(float dt,PSinfo psInfo, SpellLoading::Projectile projectileInfo, TextureInfo txtInfo, bool spellRenderer, SpellLoading::AOESpell aoeSpellInfo);
	void changeSpell(int state);

	void renderSpell();

	const Spell& getSpell(int index) const { return *spells[index]; }
	const std::vector<Spell*>& getSpells() const { return spells; }
	const uint64_t getUniqueID();
	const SpellBase* getSpellBase(OBJECT_TYPE type) const;
	bool renderSpellBool = true;

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

	PSinfo tempPS;
	TextureInfo tempTxt;

	// Don't touch if you don't know what you are doing
	friend class Client;

	//SpellEditor
	int activespell;
};