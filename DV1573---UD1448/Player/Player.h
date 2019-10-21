#pragma once
#include <Pch/Pch.h>
#include <Spells/AttackSpell.h>

#include <Spells/EnhanceAttackSpell.h>
#include <Spells/SpellHandler.h>

#include "System/BulletPhysics.h"
class Client;

class Player
{
public:
	Player(BulletPhysics* bp, std::string name, glm::vec3 playerPosition, Camera* camera, SpellHandler* spellHandler);
	~Player();

	void update(float deltaTime);
	void move(float deltaTime);
	void attack(float deltaTime);
	void createRay(); //create ray for spells
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
	std::string m_name;
	glm::vec3 m_directionVector;
	glm::vec3 m_playerPosition;
	glm::vec3 m_inputVector;
	glm::vec3 m_moveDir;
	Camera* m_playerCamera;

	SpellHandler* m_spellhandler;
	EnhanceAttackSpell m_enhanceAttack;

	SPELL_TYPE m_spellType;
	SPELL_TYPE m_specialSpelltype;
	SPELL_TYPE m_specialSpellType2;
	
	float m_attackCooldown;
	float m_specialCooldown;
	float m_special2Cooldown;

	float m_spellSpeed = 1;
	float m_speed;

	int m_nrOfSpells;
	int m_health;
	int m_frameCount;

	std::string name;
	SPELL_TYPE spellType;

	//removed in bulletPhysics.cpp
	BulletPhysics* m_bp;
	btKinematicCharacterController* m_character;
	Client* m_client;


};