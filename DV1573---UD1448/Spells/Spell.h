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
	float getCooldown();
	float getSpellSpeed();
	float getTravelTime();
	glm::vec3 getSpellPos();
	glm::vec3 getDirection();
	const uint64_t& getUniqueID() const;

	//----Set-----//
	void setDamage(int damage);
	void setSpellSpeed(float speed);
	void setSpellPos(glm::vec3 pos);
	void setCooldown(float cooldown);
	void setTravelTime(float travelTime);
	void setDirection(glm::vec3 direction);
	void setUniqueID(const uint64_t& uniqueID);

	void update(float dt);

	void destroySpell();
	

private:
	uint64_t m_uniqueID = 0;
	int spellDamage;
	float spellSpeed; 
	float spellCoolDown = 0;
	float travelTime = 0;
	glm::vec3 direction;
	glm::vec3 spellPosition = glm::vec3(0,0,0);
	
	
};