#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/Spell.h>

class AttackSpell : public Spell
{
public:
	AttackSpell(glm::vec3 pos, glm::vec3 direction, const SpellBase* spellBase);
	AttackSpell(glm::vec3 pos, OBJECT_TYPE type);
	~AttackSpell();

	// Virtual functions
	virtual const bool& getHasCollided() const;
	virtual void hasCollided();
	virtual void update(float deltaTime);
	virtual const float getDamage();
	virtual const glm::vec3& getPos() const;

private:

	bool m_hasCollided = false;
	bool m_shouldAddBounce = true;	
	int m_bounceCounter = 0;
	float m_bounceTime = 0;
};