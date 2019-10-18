#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/Spell.h>
#include <Spells/Spells.h>

class AttackSpell : public Spell
{
public:
	AttackSpell(glm::vec3 pos, glm::vec3 direction, const AttackSpellBase* spellBase);
	~AttackSpell();
//<<<<<<< HEAD
//	void updateActiveSpell(float deltaTime, btRigidBody* body);
//	void createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector);
//	void spellCooldownUpdate(float deltaTime);
//	void renderAttackSpell(std::vector<AttackSpell> test);
//
//	void update(float dt, );
//=======
	virtual void update(float deltaTime);
	void updateRigidbody(float deltaTime, btRigidBody* body);

private:
	const AttackSpellBase* m_spellBase;
};