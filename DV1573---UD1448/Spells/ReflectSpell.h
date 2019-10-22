#ifndef _REFLECTSPELL_H
#define _REFLECTSPELL_H
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/Spell.h>

class ReflectSpell : public Spell
{
public:
	ReflectSpell(glm::vec3 pos, glm::vec3 direction, const ReflectSpellBase* spellBase);
	~ReflectSpell();

	virtual void update(float deltaTime);
	void updateRigidbody(float deltaTime, btRigidBody* body);
	const float getDamage();

	void updateReflection(float deltaTime, btRigidBody* body, glm::vec3 position, glm::vec3 direction);
	bool checkReflectCollision(glm::vec3 position, glm::vec3 direction, float radius);
	const ReflectSpellBase* getBase() { return m_spellBase; }
	
private:
	const ReflectSpellBase* m_spellBase;

	Transform m_transform;
	glm::vec3 m_direction;
	float hitboxRadius;

};

#endif

