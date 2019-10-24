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

	// Virtual functions
	virtual const bool& getHasCollided() const;	//TODO: Construct this function
	virtual void hasCollided();					//TODO: Construct this function
	virtual void update(float deltaTime);
	virtual void updateRigidbody(float deltaTime, btRigidBody* body);
	virtual const float getDamage();

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

