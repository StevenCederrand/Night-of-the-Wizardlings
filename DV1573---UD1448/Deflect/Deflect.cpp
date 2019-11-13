#include <Pch/Pch.h>
#include "Deflect.h"

Deflect::Deflect()
	: GameObject()
{
	m_type = 0;
}

Deflect::Deflect(std::string name)
	: GameObject(name)
{
	m_type = 0;
}

Deflect::~Deflect()
{

}

void Deflect::update(float dt)
{

}
