#include <Pch/Pch.h>
#include "ReflectSpell.h"

ReflectSpell::ReflectSpell(glm::vec3 pos, glm::vec3 direction, const ReflectSpellBase* spellBase) 
	: Spell(pos, direction)
{
	m_type = SPELL_TYPE::REFLECT;
	m_spellBase = spellBase;

	setTravelTime(spellBase->m_lifeTime);
	setWorldPosition(pos);
	setDirection(direction);
}

ReflectSpell::~ReflectSpell()
{
}

void ReflectSpell::update(float deltaTime)
{
}

void ReflectSpell::updateRigidbody(float deltaTime, btRigidBody* body)
{
}

const float ReflectSpell::getDamage()
{
	return 0.0f;
}
