#include "Pch/Pch.h"
#include "fire.h"

fire::fire(glm::vec3 pos, glm::vec3 direction, const SpellBase* spellBase)
	: Spell(pos, direction, spellBase)
{
	m_type = OBJECT_TYPE::FIRE;
	setTravelTime(spellBase->m_lifeTime);
	Transform tempTransform;
	tempTransform.scale = glm::vec3(8.0f, 8.0f, 8.0f);
	setTransform(tempTransform);

	setWorldPosition(pos);
	setDirection(direction);
}

fire::fire(glm::vec3 pos)
	: Spell(pos, glm::vec3(0), nullptr)
{
	m_type = OBJECT_TYPE::FIRE;

	Transform tempTransform;
	tempTransform.scale = glm::vec3(8.0f, 8.0f, 8.0f);
	setTransform(tempTransform);

	setWorldPosition(pos);
}


fire::~fire()
{

}

const bool& fire::getHasCollided() const
{
	return true;

}

void fire::hasCollided()
{
}

void fire::update(float dt)
{
	fireTimer = fireTimer - dt;
	if (fireTimer <= 0)
	{
		setTravelTime(0);
	}
}

const float fire::getDamage()
{
	return m_spellBase->m_damage;
}

const glm::vec3& fire::getPos() const
{
	return glm::vec3(0);
}

void fire::updateTool(float radius, float speed, float dt)
{
}
