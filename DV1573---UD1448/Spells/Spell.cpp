#include "Pch/Pch.h"
#include "Spell.h"

Spell::Spell()
	: GameObject()
{
}

Spell::Spell(std::string name)
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

void Spell::setDamage(int damage)
{
	this->spellDamage = damage;
}

void Spell::setSpellSpeed(float speed)
{
	this->spellSpeed = speed;
}
