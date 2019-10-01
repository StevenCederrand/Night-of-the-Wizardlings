#include "Pch/Pch.h"
#include "Player.h"
#include <Networking/Client.h>


Player::Player(std::string name, glm::vec3 playerPosition, Camera *camera)
{
	if (camera == NULL) {
		 playerCamera = new Camera();
	}
	this->playerCamera = camera;
	this->playerPosition = playerPosition;
	this->name = name;
	this->speed = 5;
	this->health = 100;
	this->attackCooldown = 0;
	this->nrOfSpells = 0;
	this->directionVector = glm::vec3(0, 0, 0);
	this->moveDir = glm::vec3(0.0f);
	tempSpell = new AttackSpell("Spell", playerPosition, directionVector, 50, 2, "TestSphere.mesh");
}

Player::~Player()
{
	delete playerCamera;
	delete tempSpell;
}

void Player::update(float deltaTime)
{

	move(deltaTime);
	attack(deltaTime);
	updateAttack(deltaTime);
}

void Player::move(float deltaTime)
{
	glm::vec3 camFace = playerCamera->getCamFace();
	glm::vec3 camRight = playerCamera->getCamRight();

	camFace.y = 0;

	moveDir = glm::vec3(0.0f);

	if (glfwGetKey(playerCamera->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
	{
		moveDir -= camRight;
	}
	if (glfwGetKey(playerCamera->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
	{
		moveDir += camRight;
	}
	if (glfwGetKey(playerCamera->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
	{
		moveDir += camFace;
	}
	if (glfwGetKey(playerCamera->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
	{
		moveDir -= camFace;
	}

	if (glm::length(moveDir) >= 0.1f)
		moveDir = glm::normalize(moveDir);

	inputVector = moveDir;
	playerPosition += inputVector * speed * deltaTime;
	setPlayerPos(playerPosition);
	playerCamera->setCameraPos(playerPosition);
	playerCamera->update(playerCamera->getWindow());
}

void Player::attack(float deltaTime)
{
	if (glfwGetMouseButton(playerCamera->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && attackCooldown <= 0)
	{
		createRay();
		AttackSpell tempSpell2 = *tempSpell;
		tempSpell2.setSpellPos(glm::vec3(playerPosition.x, playerPosition.y - 1.8f, playerPosition.z) + directionVector); //-1.8 = spwn point for spell, spell need to be 0 and playerPos is set to (0,1.8,0)
		tempSpell2.translate(tempSpell2.getSpellPos());
		tempSpell2.setDirection(directionVector);
		normalSpell.push_back(tempSpell2);
		attackCooldown = 1.0f;
	}

	if(attackCooldown > 0)
		attackCooldown = attackCooldown - 1 * deltaTime;
}

void Player::updateAttack(float deltaTime)
{
	for (size_t i = 0; i < normalSpell.size(); i++)
	{
		normalSpell[i].translate(normalSpell[i].getDirection() * deltaTime * normalSpell[i].getSpellSpeed());
		normalSpell[i].setTravelTime(normalSpell[i].getTravelTime() - 1 * deltaTime);

		if (normalSpell[i].getTravelTime() <= 0)
		{
			normalSpell.erase(normalSpell.begin() + i);
		}
	}
}

void Player::createRay()
{
	float x = (2.0f * static_cast<float>(playerCamera->getXpos())) / SCREEN_WIDTH - 1.0f;
	float y = 1.0f - (2.0f * static_cast<float>(playerCamera->getYpos())) / SCREEN_HEIGHT;
	float z = 1.0f;

	//-----Spaces-----//
	glm::vec3 rayNDC = glm::vec3(x, y, z);
	glm::vec4 rayClip = glm::vec4(rayNDC.x, rayNDC.y, -1.0f, 1.0f);
	glm::vec4 rayEye = inverse(playerCamera->getProjMat()) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
	glm::vec4 rayWorldTemp = glm::vec4(inverse(playerCamera->getViewMat()) * rayEye);
	directionVector = normalize(glm::vec3(rayWorldTemp.x, rayWorldTemp.y, rayWorldTemp.z));
}

void Player::renderSpell()
{
	Client::getInstance()->updatePlayerData(this);


	for (AttackSpell object : normalSpell)
	{
		object.bindMaterialToShader("Basic_Forward");
		Renderer::getInstance()->render(object);
	}
}

void Player::setPlayerPos(glm::vec3 pos)
{
	this->playerPosition = pos;
}

void Player::spawnPlayer(glm::vec3 pos)
{
	this->playerPosition = pos;
}

void Player::setHealth(int health)
{
	this->health = health;
}

void Player::setSpeed(float speed)
{
	this->speed = speed;
}

glm::vec3 Player::getPlayerPos() const
{
	return this->playerPosition;
}

int Player::getHealth() const
{
	return this->health;
}

Camera* Player::getCamera()
{
	return playerCamera;
}

std::string Player::getName() const
{
	return this->name;
}

bool Player::isDead()
{
	if (this->health <= 0)
	{
		return true;
	}
	else
		return false;
}
