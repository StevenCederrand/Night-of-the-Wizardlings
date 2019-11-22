#include <Pch/Pch.h>
#include "EnemyShieldObject.h"

EnemyShieldObject::EnemyShieldObject()
	: GameObject()
{
	m_type = 0;
}

EnemyShieldObject::EnemyShieldObject(std::string name)
{
	m_type = 0;
}

EnemyShieldObject::~EnemyShieldObject()
{
}

void EnemyShieldObject::update(float dt)
{
}
