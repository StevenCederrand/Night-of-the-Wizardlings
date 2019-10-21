#include <Pch/Pch.h>
#include "ReflectSpell.h"

ReflectSpell::ReflectSpell(glm::vec3 pos, glm::vec3 direction, const ReflectSpellBase* spellBase) 
	: Spell(pos, direction)
{
	m_type = SPELL_TYPE::REFLECT;
	m_spellBase = spellBase;
	hitboxRadius = spellBase->m_radius;

	Transform tempTransform;
	tempTransform.scale = glm::vec3(0.1f, 0.1f, 0.1f);
	setTransform(tempTransform);
	m_transform = tempTransform;

	setTravelTime(spellBase->m_lifeTime);
	setWorldPosition(pos);
	setDirection(direction);
}

ReflectSpell::~ReflectSpell()
{
}

void ReflectSpell::update(float deltaTime)
{
	setTransform(m_transform);
	setTravelTime(getTravelTime() - deltaTime);
}

void ReflectSpell::updateRigidbody(float deltaTime, btRigidBody* body)
{
}

const float ReflectSpell::getDamage()
{
	return 0.0f;
}

void ReflectSpell::updateReflection(float deltaTime, btRigidBody* body, glm::vec3 position, glm::vec3 direction)
{
	m_transform.position = position + (direction * 2);
	m_direction = glm::normalize(direction);
}

bool ReflectSpell::checkReflectCollision(glm::vec3 position, glm::vec3 direction, float radius)
{
	// TODO: not set here
	float sphereThis = 1.0f;

	int radiusSquared = sphereThis + radius;
	radiusSquared *= radiusSquared;

	glm::vec3 difVec = m_transform.position - position;

	float distance = glm::sqrt(
		difVec.x * difVec.x +
		difVec.y * difVec.y +
		difVec.z * difVec.z);
	
	if (distance <= radiusSquared)
	{
		float angle = glm::acos(glm::dot(m_direction, glm::normalize(direction)));
		logTrace("Collision angle: {0}", angle);

		if (glm::acos(glm::dot(m_direction, glm::normalize(difVec))) >= 90.0f)
		{



			return true;
		}
	}
	return false;
}
