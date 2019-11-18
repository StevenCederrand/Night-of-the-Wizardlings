#include <Pch/Pch.h>
#include "DeflectObject.h"

DeflectObject::DeflectObject()
	: GameObject()
{
	m_type = 0;
}

DeflectObject::DeflectObject(std::string name)
	: GameObject(name)
{
	m_type = 0;
}

DeflectObject::~DeflectObject()
{

}

void DeflectObject::update(float dt)
{

}
