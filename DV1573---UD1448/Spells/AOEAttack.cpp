#include "Pch/Pch.h"
#include "AOEAttack.h"

AOEAttack::AOEAttack(glm::vec3 pos, glm::vec3 direction, const FlamestrikeSpellBase* spellBase)
	: Spell(pos, direction)
{
	m_type = OBJECT_TYPE::FLAMESTRIKE;
	m_spellBase = spellBase;
	setTravelTime(spellBase->m_lifeTime);

	//Transform tempTransform;
	//tempTransform.scale = glm::vec3(0.2f, 0.2f, 0.2f);
	//setTransform(tempTransform);

	setWorldPosition(pos);
	setDirection(direction);
}

AOEAttack::~AOEAttack()
{

}

const bool& AOEAttack::getHasCollided() const
{
	return true;
}

void AOEAttack::hasCollided()
{
}

void AOEAttack::updateActiveSpell(float deltaTime)
{
	
	//if (m_updateSpellPos == true)
	//{
	//	setSpellPos(getDirection() * deltaTime * getSpellSpeed());
	//	newVer += getSpellPos();
	//	setSpellPos(newVer);
	//}
	//
	////updateSpellPos(getSpellPos());

	setDirection(getDirection() + deltaTime * gravityVector);

	//if (getSpellPos().y >= 0)
	//{
	//	translate(getDirection());
	//}
	//	
	//std::cout << getSpellPos().x << " " << getSpellPos().y << " " << getSpellPos().z << std::endl;

	//setTravelTime(getTravelTime() - 1 * deltaTime);

	////AOE
	//if (getSpellPos().y <= 0)
	//{
	//	m_updateSpellPos = false;

	//	radiusVector = getSpellPos() + glm::vec3(5, 0, 0);

	//	fireIsGone += 1 * deltaTime;
	//	if (fireIsGone >= 5)
	//		setTravelTime(0);

	//}
}

void AOEAttack::updateRigidbody(float deltaTime, btRigidBody* body)
{
	btVector3 pos2 = btVector3(
		getDirection().x,
		getDirection().y,
		getDirection().z) * m_spellBase->m_speed;
	body->setLinearVelocity(pos2);

	btVector3 pos = body->getWorldTransform().getOrigin();
	setWorldPosition(glm::vec3(pos.getX(), pos.getY(), pos.getZ()));
}


void AOEAttack::update(float dt)
{
	setTravelTime(getTravelTime() - dt);
}

const float AOEAttack::getDamage()
{
	return m_spellBase->m_damage;
}
