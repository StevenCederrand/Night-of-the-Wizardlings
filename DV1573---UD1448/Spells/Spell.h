#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>

class Spell : public GameObject
{
public:
	Spell(glm::vec3 pos, glm::vec3 m_direction);
	~Spell();

	//-----Get-----//
	//float getCooldown();
	//float getSpellSpeed();
	float getTravelTime() { return m_travelTime; }
	glm::vec3 getDirection() { return m_direction; }
	//----Set-----//
	//void setDamage(int damage);
	//void setSpellSpeed(float speed);
	//void setSpellPos(glm::vec3 pos);
	//void setCooldown(float cooldown);
	void setTravelTime(float m_travelTime);
	void setDirection(glm::vec3 m_direction);
	const int& getType() const { return m_type; }

	virtual void update(float dt) = 0;

private:
	//int m_spellDamage;
	//float m_spellSpeed; 
	//float m_spellCoolDown = 0;
	float m_travelTime;
	glm::vec3 m_direction;
	
protected:
	int m_type;
};