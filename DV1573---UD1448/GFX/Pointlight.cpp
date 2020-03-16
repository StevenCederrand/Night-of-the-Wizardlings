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
	setAttenuationAndRadius(glm::vec4(1.0f, 0.09f, 0.032f, 10)); //Default Attenation values

}

Pointlight::~Pointlight()
{
}

void Pointlight::setAttenuationAndRadius(const glm::vec4 attenAndRadius)
{
	m_attenAndRadius = attenAndRadius;
}

void Pointlight::setColor(glm::vec3 color)
{
	m_color = color;
}

void Pointlight::update(float dt) {

}

const glm::vec4& Pointlight::getAttenuationAndRadius() const
{
	return m_attenAndRadius;
}

const glm::vec3& Pointlight::getColor() const
{
	return m_color;
}
