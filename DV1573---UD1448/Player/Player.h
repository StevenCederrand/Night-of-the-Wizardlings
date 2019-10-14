#pragma once
#include <Pch/Pch.h>
#include <Spells/AttackSpell.h>

#include <Spells/EnhanceAttackSpell.h>
#include <Spells/SpellHandler.h>


#include "System/BulletPhysics.h"
#include <Bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <Bullet/BulletCollision/CollisionDispatch/btGhostObject.h>


class Player
{
public:
	Player(BulletPhysics* bp, std::string name = "", glm::vec3 playerPosition = glm::vec3(0.0f, 0.0f, 0.0f), Camera* camera = NULL);
	~Player();

	void update(float deltaTime);
	void playerJump();
	void move(float deltaTime);
	void attack(float deltaTime);
	void createRay(); //create ray for spells
	void renderSpell();
	void spawnPlayer(glm::vec3 pos);

	void createRigidBody(BulletPhysics* bp);
	void forceUp();
	bool isDead();

	//-----Get-----//
	Camera* getCamera();
	glm::vec3 getPlayerPos() const;
	int getHealth() const;
	std::string getName() const;

	//-----Set-----//
	void setPlayerPos(glm::vec3 pos);
	void setHealth(int health);
	void setSpeed(float speed);

private:
	SpellHandler* spellhandler;

	glm::vec3 m_directionVector;
	glm::vec3 m_playerPosition;
	glm::vec3 m_inputVector;
	glm::vec3 m_moveDir;

	Camera* m_playerCamera;
	btRigidBody* m_body;
	float m_attackCooldown;
	float m_spellSpeed = 1;
	float m_speed;
	int m_nrOfSpells;
	int m_health;
	int m_frameCount;
	std::string m_name;

	TYPE m_spellType;
	BulletPhysics* m_bp;
	btKinematicCharacterController* controller;


};