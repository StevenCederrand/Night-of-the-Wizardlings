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
	this->spellType = ENHANCEATTACK;
	spellhandler = new SpellHandler(playerPosition, directionVector);

}

Player::~Player()
{
	delete playerCamera;
	delete spellhandler;
}

void Player::update(float deltaTime)
{
	selectSpell();
	move(deltaTime);
	spellhandler->spellUpdate(deltaTime);
	attack(deltaTime);
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

	if (glm::length(moveDir) >= 0.01f)
		moveDir = glm::normalize(moveDir);

	playerPosition += moveDir * speed * deltaTime;
	setPlayerPos(playerPosition);
	playerCamera->setCameraPos(playerPosition);
	playerCamera->update(playerCamera->getWindow());
}

void Player::attack(float deltaTime)
{
	if (glfwGetMouseButton(playerCamera->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)// && tempSpell->getCooldown() <= 0
	{
		createRay();
		spellhandler->createSpell(deltaTime, playerPosition, directionVector, spellType);
	}
	spellhandler->spellCooldown(deltaTime);

}

void Player::createRay()
{
	float x = (2.0f * playerCamera->getXpos()) / SCREEN_WIDTH - 1.0f;
	float y = 1.0f - (2.0f * playerCamera->getYpos()) / SCREEN_HEIGHT;
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

	spellhandler->renderSpell();

}

void Player::setPlayerPos(glm::vec3 pos)
{
	this->playerPosition = pos;
}

void Player::spawnPlayer(glm::vec3 pos)
{
	this->playerPosition = pos;
}

void Player::selectSpell()
{
	if (glfwGetKey(playerCamera->getWindow(), GLFW_KEY_1) == GLFW_PRESS)
	{
		this->spellType = NORMALATTACK;
	}

	if (glfwGetKey(playerCamera->getWindow(), GLFW_KEY_2) == GLFW_PRESS)
	{
		this->spellType = ENHANCEATTACK;
	}
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
