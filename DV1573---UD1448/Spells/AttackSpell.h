#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/Spell.h>

class AttackSpell : public Spell
{
public:
	AttackSpell(glm::vec3 pos, glm::vec3 direction, const AttackSpellBase* spellBase);
	AttackSpell(glm::vec3 pos, OBJECT_TYPE type);
	~AttackSpell();

	// Virtual functions
	virtual const bool& getHasCollided() const;
	virtual void hasCollided();
	virtual void update(float deltaTime);
	virtual void updateRigidbody(float deltaTime);
	virtual const float getDamage();
	virtual const glm::vec3& getPos() const;

	const AttackSpellBase* getBase() { return m_spellBase; }

private:
	const AttackSpellBase* m_spellBase;
	//int m_localBounce = 0;
	//int m_nrOfBounce = 0;
	bool m_hasCollided = false;
	int m_bounceCounter = 0;
	float m_bounceTime = 0;
	bool m_shouldAddBounce = true;	


};