#pragma once
#include <Pch/Pch.h>
#include <Spells/AttackSpell.h>
#include <GameObject/AnimatedObject.h>
#include <Spells/EnhanceAttackSpell.h>
#include <Spells/SpellHandler.h>
#include "System/BulletPhysics.h"
#include <System/SoundHandler.h>
#include <HUD/HudHandler.h>
class Client;

class Player
{

public:
	Player(std::string name, glm::vec3 playerPosition, Camera* camera, SpellHandler* spellHandler);
	~Player();

	void update(float deltaTime);
	void updateListenerProperties();
	void attack();
	void spawnPlayer(glm::vec3 pos);
	void updateMesh();
	void onDead();
	void onRespawn();
	void increaseMana(const float& increase);
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
	const float& getMaxAttackCooldown() const;
	const float& getMaxSpecialCooldown() const;
	const glm::vec3 getMeshHalfSize() const;
	const float& getMana() const;
	const bool& onGround() const;
	const OBJECT_TYPE& currentSpell() const; //Get info over what spell should be in use. Either triple or flamestrike

	//-----Set-----//
	void setPlayerPos(glm::vec3 pos);
	void setHealth(int health);
	void setMana(int mana);
	void setSpeed(float speed);
	void logicStop(const bool& stop);
	void submitHudHandler(HudHandler* hudHandler);

private:
	void move(float deltaTime); 	

	std::string m_name;
	glm::vec3 m_directionVector;
	glm::vec3 m_playerPosition;
	glm::vec3 m_cameraPosition;
	glm::vec3 m_spellSpawnPosition;
	
	glm::vec3 m_moveDir;
	Camera* m_playerCamera;
	bool m_logicStop;
	SpellHandler* m_spellhandler;
	EnhanceAttackSpell m_enhanceAttack;
	AnimatedObject* m_firstPersonMesh;
	GameObject* m_shieldObject;

	// This is used for swapping spells.
	// So that we know if we are to swap to flamestrike or triple spell
	bool m_usingTripleSpell; 
	HudHandler* m_hudHandler;

	float m_attackCooldown;
	float m_deflectCooldown;
	float m_specialCooldown;
	float m_spellSpeed = 1;
	float m_maxSpeed;
	float m_deflectSoundGain = 1.0f;
	
	bool m_rMouse; //Right Mouse
	float m_mana;
	int m_health;
	int m_frameCount;

	float m_maxHealth;
	float m_maxMana; 
	float m_manaRegen;
	float m_maxAttackCooldown;
	float m_deflectManaDrain;
	float m_maxDeflectCooldown;
	float m_specialManaDrain;
	float m_maxSpecialCooldown;

	OBJECT_TYPE m_mainAtkType;
	OBJECT_TYPE m_specialAtkType;
	
	bool m_deflecting;
	bool m_isWalking;
	bool m_isJumping;
	
	AnimationState animState;
	void PlayAnimation(float deltaTime);

	btKinematicCharacterController* m_character;
	Client* m_client;
};