#include "Pch/Pch.h"


Pointlight::Pointlight() : GameObject() 
{
	setWorldPosition(glm::vec3(0));
	m_color = glm::vec3(1);
}

Pointlight::Pointlight(glm::vec3 position, glm::vec3 color) : GameObject("POINTLIGHT")
{
	setWorldPosition(position);
	m_color = color; 
	m_type = OBJECT_TYPE::POINTLIGHT;
}

Pointlight::~Pointlight()
{
}

void Pointlight::update(float dt) {

}