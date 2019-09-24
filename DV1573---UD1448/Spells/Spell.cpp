#include "Pch/Pch.h"
#include "Spell.h"

Spell::Spell(glm::vec3 pos)
	: GameObject()
{
	this->spellPosition = pos;
}

Spell::Spell(std::string name, glm::vec3 pos)
	: GameObject(name)
{
}

Spell::~Spell()
{
}

void Spell::CreateSpellObject()
{
}

void Spell::updateSpell()
{

}

int Spell::getCooldown()
{
	return this->spellCoolDown;
}

void Spell::setDamage(int damage)
{
	this->spellDamage = damage;
}

void Spell::setSpellSpeed(float speed)
{
	this->spellSpeed = speed;
}

void Spell::setSpellPos(glm::vec3 pos)
{
	this->spellPosition = pos;
}

void Spell::setCooldown(int cooldown)
{
	this->spellCoolDown = cooldown;
}

void Spell::update(float dt)
{
}

void Spell::destroySpell()
{
}
