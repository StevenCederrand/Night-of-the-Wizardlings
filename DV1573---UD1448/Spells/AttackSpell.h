#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/Spell.h>
#include <spellLoader.h>

class AttackSpell : public Spell
{
public:
	AttackSpell(glm::vec3 pos, glm::vec3 direction, AttackSpellBase* spellBase);
	AttackSpell(glm::vec3 pos, OBJECT_TYPE type);
	~AttackSpell();

	// Virtual functions
	virtual const bool& getHasCollided() const;
	virtual void hasCollided();
	virtual void update(float deltaTime);
	virtual void updateRigidbody(float deltaTime, btRigidBody* body);
	virtual const float getDamage();
	virtual const glm::vec3& getPos() const;
    void updateTool(float radius, float speed, float dt);

	const AttackSpellBase* getBase() { return m_spellBase; }

private:
	AttackSpellBase* m_spellBase;
    Transform tempTransformTest;
    glm::vec3 m_tempScale = glm::vec3(0.2f, 0.2f, 0.2f);
    float updateScale = 0;
    float updateSpeed = -2;
    SpellLoader mySpellLoader;
	//int m_localBounce = 0;
	//int m_nrOfBounce = 0;
	bool m_hasCollided = false;
	int m_bounceCounter = 0;
	float m_bounceTime = 0;
	bool m_shouldAddBounce = true;	


};
