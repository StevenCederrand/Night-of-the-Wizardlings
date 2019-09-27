#pragma once
#include <Pch/Pch.h>
#include <Spells/AttackSpell.h>

class Player
{
public:
	Player(std::string name = "", glm::vec3 playerPosition = glm::vec3(0.0f, 0.0f, 0.0f), Camera* camera = NULL);
	~Player();

	void update(float deltaTime);
	void playerJump();
	void move(float deltaTime);
	void attack(float deltaTime);
	void updateAttack(float deltaTime);
	void createRay(); //create ray for spells

	void setPlayerPos(glm::vec3 pos);
	void spawnPlayer(glm::vec3 pos);

	void setHealth(int health);
	void setSpeed(float speed);
	glm::vec3 getPlayerPos() const;
	int getHealth() const;
	Camera* getCamera();
	std::string getName() const;
	bool isDead();


private:
	glm::vec3 playerPosition;
	glm::vec3 inputVector;
	int health;
	float speed;
	Camera* playerCamera;
	float spellSpeed = 1;
	glm::vec3 moveDir;
	glm::vec3 directionVector;
	std::vector<AttackSpell*> normalSpell;
	std::string name;
	float attackCooldown;
	int nrOfSpells;

};