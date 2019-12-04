#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include "Spells.h"

class Spell : public GameObject
{
public:
	Spell(glm::vec3 pos, glm::vec3 m_direction);
	~Spell();

	float getTravelTime() { return m_travelTime; };
	const glm::vec3& getDirection() const;
	const uint64_t& getUniqueID() const;
	const int& getType() const { return m_type; }	
	const int& getSoundSlot() const;
	
	//----Set-----//
	void setUniqueID(const uint64_t& uniqueID);
	void setTravelTime(float m_travelTime);
	void setDirection(glm::vec3 m_direction);
	void setType(int type);
	void setBodyReference(btRigidBody* body);
	void setSoundSlot(int slot);

	btRigidBody* getBodyReference() { return m_bodyRef; }
	// Virtual functions
	virtual const bool& getHasCollided() const = 0;
	virtual void hasCollided() = 0;
	virtual void update(float dt) = 0;
	virtual void updateRigidbody(float deltaTime, btRigidBody* body) = 0;
	virtual const float getDamage() = 0;
	virtual const glm::vec3& getPos() const = 0;
    virtual void updateTool(float radius, float speed, float dt);

		
private:
	uint64_t m_uniqueID = 0;
	float m_travelTime;
	glm::vec3 m_direction;
	int m_soundSlot = 0;

	btRigidBody* m_bodyRef;
};
