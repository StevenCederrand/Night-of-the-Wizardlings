#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <GameObject/WorldObject.h>
#include <Spells/Spell.h>

class fire : public Spell
{
public:
	fire(glm::vec3 pos, glm::vec3 direction, const FireSpellBase* spellBase);
	fire(glm::vec3 pos);
	~fire();

	virtual const bool& getHasCollided() const;
	virtual void hasCollided();
	virtual void updateRigidbody(float deltaTime);
	virtual void update(float dt);
	virtual const float getDamage();
	virtual const glm::vec3& getPos() const;



private:
	const FireSpellBase* m_spellBase;
	float fireTimer = 8;

};