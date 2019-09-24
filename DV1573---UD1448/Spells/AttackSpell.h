#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/Spell.h>

class AttackSpell : public Spell
{
public:
	AttackSpell(glm::vec3 pos);
	AttackSpell(std::string name, glm::vec3 pos);
	~AttackSpell();

	void update(float dt);

private:

};