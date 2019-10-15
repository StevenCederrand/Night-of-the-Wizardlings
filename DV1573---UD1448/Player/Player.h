#pragma once
#include <Pch/Pch.h>
#include <Spells/AttackSpell.h>

#include <Spells/EnhanceAttackSpell.h>
#include <Spells/SpellHandler.h>

#include <System/BulletPhysics.h>



class Player
{
public:
	Player(BulletPhysics* bp, std::string name, glm::vec3 playerPosition, Camera* camera, SpellHandler* spellHandler);
	~Player();

	void update(float deltaTime);
	void playerJump();
	void move(float deltaTime);
	void attack();
	void createRay(); //create ray for spells
	void spawnPlayer(glm::vec3 pos);
	void selectSpell();
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

	glm::vec3 directionVector;
	glm::vec3 m_playerPosition;
	glm::vec3 inputVector;
	glm::vec3 moveDir;

	Camera* playerCamera;
	float attackCooldown;
	float m_spellSpeed = 1;
	float speed;
	int nrOfSpells;
	int health;
	int m_frameCount;
	std::string name;
	SPELL_TYPE spellType;

	//removed in bulletPhysics.cpp
	BulletPhysics* m_bp;
	btKinematicCharacterController* m_character;



};