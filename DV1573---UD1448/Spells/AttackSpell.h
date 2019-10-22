#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/Spell.h>

class AttackSpell : public Spell
{
public:
	AttackSpell(glm::vec3 pos, glm::vec3 direction, const AttackSpellBase* spellBase);
	~AttackSpell();

	virtual void update(float deltaTime);
	void updateRigidbody(float deltaTime, btRigidBody* body);
	const float getDamage();

	const AttackSpellBase* getBase() { return m_spellBase; }

private:
	const AttackSpellBase* m_spellBase;

};