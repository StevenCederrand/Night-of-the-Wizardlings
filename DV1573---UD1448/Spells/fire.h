#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <GameObject/WorldObject.h>
#include <Spells/Spell.h>

class fire : public Spell
{
public:
	fire(glm::vec3 pos, glm::vec3 direction, const SpellBase* spellBase);
	fire(glm::vec3 pos);
	~fire();

	virtual const bool& getHasCollided() const;
	virtual void hasCollided();
	virtual void update(float dt);
	virtual const float getDamage();
	virtual const glm::vec3& getPos() const;
	void updateTool(float radius, float speed, float dt);



private:
	Transform tempTransformTest;

	float fireTimer = 8;

};