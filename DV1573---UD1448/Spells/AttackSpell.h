#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/Spell.h>
#include <SpellLoader/spellLoader.h>

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
	void updateTool(float radius, float speed, float dt);

private:

	Transform tempTransformTest;
	glm::vec3 m_tempScale = glm::vec3(0.2f, 0.2f, 0.2f);
	float updateScale = 0;
	float updateSpeed = -2;
	SpellLoader mySpellLoader;

	bool m_hasCollided = false;
	bool m_shouldAddBounce = true;	
	int m_bounceCounter = 0;
	float m_bounceTime = 0;
};