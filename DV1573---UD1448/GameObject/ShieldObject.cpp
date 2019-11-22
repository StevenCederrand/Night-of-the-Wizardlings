#include <Pch/Pch.h>
#include "ShieldObject.h"

ShieldObject::ShieldObject()
	: GameObject()
{
	m_type = 0;
}

ShieldObject::ShieldObject(std::string name)
	: GameObject(name)
{
	m_type = 0;
}

ShieldObject::~ShieldObject()
{

}

void ShieldObject::update(float dt)
{

}
