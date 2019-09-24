#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>

class Spell : public GameObject
{
public:
	Spell(glm::vec3 pos);
	Spell(std::string name, glm::vec3 pos);
	~Spell();

	void CreateSpellObject();
	void updateSpell();

	//-----Get-----//
	int getCooldown();
	//----Set-----//
	void setDamage(int damage);
	void setSpellSpeed(float speed);
	void setSpellPos(glm::vec3 pos);
	void setCooldown(int cooldown);

	void update(float dt);

	void destroySpell();


private:
	int spellDamage;
	float spellSpeed; 
	int spellCoolDown = 0;
	glm::vec3 direction;
	glm::vec3 spellPosition;
	
};