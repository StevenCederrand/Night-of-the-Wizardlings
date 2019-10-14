#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>

class Spell : public GameObject
{
public:
	Spell(glm::vec3 pos);
	Spell(std::string name, glm::vec3 pos, glm::vec3 direction, float speed, float travelTime, std::string meshName, float cooldown);
	~Spell();

	//-----Get-----//
	float getCooldown() const;
	float getSpellSpeed() const;
	float getTravelTime() const;
	glm::vec3 getSpellPos() const;
	glm::vec3 getDirection() const;
	//----Set-----//
	void setDamage(int damage);
	void setSpellSpeed(float speed);
	void setSpellPos(glm::vec3 pos);
	void setCooldown(float cooldown);
	void setTravelTime(float travelTime);
	void setDirection(glm::vec3 direction);
	void updateSpellPos(glm::vec3 newPos);

	void update(float dt);

	void destroySpell();
	

private:
	int m_spellDamage;
	float m_spellSpeed; 
	float m_spellCoolDown = 0;
	float m_travelTime = 0;
	glm::vec3 m_direction;
	glm::vec3 m_spellPosition = glm::vec3(0,0,0);
	
	
};