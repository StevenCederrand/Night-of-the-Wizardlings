#include "Pch/Pch.h"
#include "AOEAttack.h"

AOEAttack::AOEAttack(glm::vec3 pos, glm::vec3 direction, const SpellBase* spellBase)
	: Spell(pos, direction, spellBase)
{
	m_type = OBJECT_TYPE::FLAMESTRIKE;
	setTravelTime(spellBase->m_lifeTime);

	//Transform tempTransform;
	//tempTransform.scale = glm::vec3(0.2f, 0.2f, 0.2f);
	//setTransform(tempTransform);

	setWorldPosition(pos);
	setDirection(direction);
}

AOEAttack::AOEAttack(glm::vec3 pos)
	: Spell(pos, glm::vec3(0), nullptr)
{
	m_type = OBJECT_TYPE::FLAMESTRIKE;

	//Transform tempTransform;
	//tempTransform.scale = glm::vec3(0.2f, 0.2f, 0.2f);
	//setTransform(tempTransform);
	setWorldPosition(pos);
}

AOEAttack::~AOEAttack()
{

}

const bool& AOEAttack::getHasCollided() const
{
	return m_hasCollided;
}

void AOEAttack::hasCollided()
{
	m_hasCollided = true;
}

bool AOEAttack::spellOnGround()
{
	if (m_fire)
		return true;
	else
		return false;
}

void AOEAttack::setSpellBool(bool state)
{
	m_fire = state;
}



void AOEAttack::update(float dt)
{
	setTravelTime(getTravelTime() - dt);

	btRigidBody* body = getRigidBody();

	m_bounceTime += dt;
	if (m_hasCollided)
	{
		m_hasCollided = false;

		if (m_bounceTime > 0.02f)
		{
			if (m_bounceCounter > m_spellBase->m_maxBounces)
			{
				m_fire = true;
				setTravelTime(0);
			}

			m_bounceTime = 0.0f;
			m_bounceCounter++;
		}
	}

	btVector3 pos2 = btVector3(
		getDirection().x,
		getDirection().y,
		getDirection().z) * m_spellBase->m_speed;
	//body->setLinearVelocity(pos2);

	btVector3 rigidBodyPos = body->getWorldTransform().getOrigin();

	btTransform rigidBodyTransform = body->getWorldTransform();
	Transform newTransform;
	newTransform.position.x = rigidBodyTransform.getOrigin().getX();
	newTransform.position.y = rigidBodyTransform.getOrigin().getY();
	newTransform.position.z = rigidBodyTransform.getOrigin().getZ();

	newTransform.rotation.x = rigidBodyTransform.getRotation().getX();
	newTransform.rotation.y = rigidBodyTransform.getRotation().getY();
	newTransform.rotation.z = rigidBodyTransform.getRotation().getZ();
	newTransform.rotation.w = rigidBodyTransform.getRotation().getW();

	newTransform.scale = getTransform().scale;

	setTransform(newTransform);
	m_pos = newTransform.position;
}

const float AOEAttack::getDamage()
{
	return m_spellBase->m_damage;
}

const glm::vec3& AOEAttack::getPos() const
{
	return m_pos;
}
