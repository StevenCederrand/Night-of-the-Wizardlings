#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>

class Spell : GameObject
{
public:
	Spell();
	Spell(std::string name);
	~Spell();

	void CreateSpellObject();
	void updateSpell();
	void setDamage(int damage);
	void setSpellSpeed(float speed);


private:
	int spellDamage;
	float spellSpeed; 
	float spellCoolDown;
	glm::vec3 direction;
	
};