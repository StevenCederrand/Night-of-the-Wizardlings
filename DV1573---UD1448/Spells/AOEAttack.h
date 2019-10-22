#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <GameObject/WorldObject.h>
#include <Spells/Spell.h>
#include <Spells/fire.h>

class AOEAttack : public Spell
{
public:
	AOEAttack(glm::vec3 pos, glm::vec3 direction, const FlamestrikeSpellBase* spellBase);
	~AOEAttack();

	void updateActiveSpell(float deltaTime);
	void updateRigidbody(float deltaTime, btRigidBody* body);

	void update(float dt);
	const float getDamage();


private:
	glm::vec3 gravityVector = glm::vec3(0,-1,0);
	glm::vec3 newVer = glm::vec3(0, 0, 0);
	glm::vec3 tempVer = glm::vec3(0, 0, 0);
	glm::vec3 pos;
	glm::vec3 radiusVector = glm::vec3(0, 0, 0);

	fire *tempFire;
	float fireIsGone;
	bool loadFire = true;
	bool test123 = false;
	bool m_updateSpellPos = true;

	//new---

	const FlamestrikeSpellBase* m_spellBase;
};