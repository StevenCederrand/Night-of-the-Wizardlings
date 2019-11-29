#include <Pch/Pch.h>
#include "ReflectSpell.h"

ReflectSpell::ReflectSpell(glm::vec3 pos, glm::vec3 direction, const ReflectSpellBase* spellBase) 
	: Spell(pos, direction)
{
	m_type = OBJECT_TYPE::REFLECT;
	m_spellBase = spellBase;
	hitboxRadius = spellBase->m_radius;

	Transform tempTransform;
	tempTransform.scale = glm::vec3(0.01f, 0.01f, 0.01f);
	setTransform(tempTransform);
	m_transform = tempTransform;

	setTravelTime(spellBase->m_lifeTime);
	setWorldPosition(pos);
	setDirection(direction);
}

ReflectSpell::ReflectSpell(glm::vec3 pos) : Spell(pos, glm::vec3(0))
{
	m_type = OBJECT_TYPE::REFLECT;

	Transform tempTransform;
	tempTransform.scale = glm::vec3(0.01f, 0.01f, 0.01f);
	setTransform(tempTransform);
	m_transform = tempTransform;
	setWorldPosition(pos);
}

ReflectSpell::~ReflectSpell()
{
}

const bool& ReflectSpell::getHasCollided() const
{
	return true;
}

void ReflectSpell::hasCollided()
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

const glm::vec3& ReflectSpell::getPos() const
{
	return glm::vec3(0);
}

void ReflectSpell::updateReflection(float deltaTime, btRigidBody* body, glm::vec3 position, glm::vec3 direction)
{
	m_transform.position = position + (direction * 0.4f);
	m_direction = glm::normalize(direction);
}

bool ReflectSpell::checkReflectCollision(glm::vec3 position, glm::vec3 direction, float radius)
{

	int radiusSquared = static_cast<int>(m_spellBase->m_radius + radius);
	radiusSquared *= radiusSquared;

	glm::vec3 difVec = m_transform.position - position;

	float distance = glm::sqrt(
		difVec.x * difVec.x +
		difVec.y * difVec.y +
		difVec.z * difVec.z);
	
	if (distance <= radiusSquared)
	{
		float angle = glm::dot(m_direction, direction);
		logTrace("Collision angle: {0}", angle);

		if (glm::dot(direction, m_direction) <= -0.2f)
		{
			return true;
		}
	}
	return false;
}
