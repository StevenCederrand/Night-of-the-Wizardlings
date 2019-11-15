#pragma once
#include <Pch/Pch.h>
#include <Spells/AttackSpell.h>
#include <GameObject/AnimatedObject.h>
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
	void attack();
	void createRay(); //create ray for spells
	void spawnPlayer(glm::vec3 pos);
	void updateMesh();

	bool isDead();

	//-----Get-----//
	Camera* getCamera();
	const glm::vec3& getPlayerPos() const;
	const int& getHealth() const;
	const std::string& getName() const;
	const bool& isDeflecting() const;
	const AnimationState* getAnimState() const;

	const float& getAttackCooldown() const;
	const float& getSpecialCooldown() const;
	const float& getDeflectCooldown() const;
	const float& getMaxAttackCooldown() const;
	const float& getMaxSpecialCooldown() const;
	const float& getMana() const;
	//-----Set-----//
	void setPlayerPos(glm::vec3 pos);
	void setHealth(int health);
	void setSpeed(float speed);
	void logicStop(const bool& stop);

private:
	void move(float deltaTime); 

	std::string m_name;
	glm::vec3 m_directionVector;
	glm::vec3 m_playerPosition;
	glm::vec3 m_cameraPosition;
	glm::vec3 m_spellSpawnPosition;
	//glm::vec3 m_inputVector;

	glm::vec3 m_moveDir;
	Camera* m_playerCamera;
	bool m_logicStop;
	SpellHandler* m_spellhandler;
	EnhanceAttackSpell m_enhanceAttack;
	AnimatedObject* m_firstPersonMesh;

	SPELL_TYPE m_spellType;
	SPELL_TYPE m_specialSpelltype;
	SPELL_TYPE m_specialSpellType2;
	SPELL_TYPE m_specialSpellType3;
	
	float m_attackCooldown;
	float m_deflectCooldown;
	float m_specialCooldown;
	float m_special3Cooldown;
	float m_maxAttackCooldown, m_maxSpecialCooldown;
	float m_spellSpeed = 1;
	float m_speed;
	
	float m_mana;

	int m_nrOfSpells;
	int m_health;
	int m_frameCount;

	std::string name;

	float m_timeLeftInDeflectState;
	bool m_deflecting;

	AnimationState animState;
	void PlayAnimation(float deltaTime);
	//removed in bulletPhysics.cpp
	BulletPhysics* m_bp;
	btKinematicCharacterController* m_character;
	Client* m_client;
};