#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include "Spells.h"

class Spell : public GameObject
{
public:
	Spell(glm::vec3 pos, glm::vec3 m_direction);
	~Spell();

	float getTravelTime() { return m_travelTime; }
	glm::vec3 getDirection() { return m_direction; }
	const uint64_t& getUniqueID() const;
	
	//----Set-----//

	void setUniqueID(const uint64_t& uniqueID);
	void setTravelTime(float m_travelTime);
	void setDirection(glm::vec3 m_direction);
	const int& getType() const { return m_type; }

	virtual void update(float dt) = 0;
	virtual void updateRigidbody(float deltaTime, btRigidBody* body) = 0;
	virtual const float getDamage() = 0;
private:
	uint64_t m_uniqueID = 0;
	float m_travelTime;
	glm::vec3 m_direction;

protected:
	int m_type;
};