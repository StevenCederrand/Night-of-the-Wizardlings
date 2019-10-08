#include "Pch/Pch.h"
#include "Player.h"
#include <Networking/Client.h>



Player::Player(BulletPhysics* bp, std::string name, glm::vec3 playerPosition, Camera *camera)
{
	if (camera == NULL) {
		 playerCamera = new Camera();
	}
	this->playerCamera = camera;
	this->m_playerPosition = playerPosition;
	this->name = name;
	this->speed = 5;
	this->health = 100;
	this->attackCooldown = 0;
	this->nrOfSpells = 0;
	this->directionVector = glm::vec3(0, 0, 0);
	this->moveDir = glm::vec3(0.0f);

	//m_frameCount = 0;
	//tempSpell = new AttackSpell("Spell", playerPosition, directionVector, 50, 2, "TestSphere.mesh");

	this->spellType = NORMALATTACK;
	spellhandler = new SpellHandler(playerPosition, directionVector);


	m_bp = bp;
	m_character = m_bp->createCharacter();
}

Player::~Player()
{
	delete playerCamera;
	delete spellhandler;
}

void Player::update(float deltaTime)
{
	m_character->updateAction(m_bp->getDynamicsWorld(), deltaTime);
	selectSpell();
	move(deltaTime);
	spellhandler->spellUpdate(deltaTime);
	attack(deltaTime);
	Client* client = Client::getInstance();
	client->updatePlayerData(this);

	if (Input::isKeyReleased(GLFW_KEY_E)) {
		client->sendStartRequestToServer();
	}

}


void Player::move(float deltaTime)
{
	m_frameCount++;
	if (m_frameCount < 5)
	{
		return;
	}
	glm::vec3 camFace = playerCamera->getCamFace();
	glm::vec3 camRight = playerCamera->getCamRight();

	btVector3 totalForce = m_character->getLinearVelocity();
	
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

	m_playerPosition += moveDir * speed * deltaTime;
	setPlayerPos(m_playerPosition);
	playerCamera->setCameraPos(m_playerPosition);
	if (glfwGetKey(playerCamera->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{		
		if (m_character->canJump())
		{			
			m_character->jump(btVector3(0.0f, 3.0f, 0.0f));
		}					
	}	

	//move the physics box
	btScalar yValue = std::ceil(m_character->getLinearVelocity().getY()*100.0) / 100.0;	//Round to two decimals
	btVector3 translate = btVector3
		(moveDir.x * speed * deltaTime,
		yValue,
		moveDir.z * speed * deltaTime);
	m_character->setLinearVelocity(translate);

	//update playercamera position
	btVector3 playerPos = m_character->getGhostObject()->getWorldTransform().getOrigin();
	m_playerPosition = glm::vec3(playerPos.getX(), playerPos.getY()* 2, playerPos.getZ());
	
	playerCamera->setCameraPos(m_playerPosition);
	playerCamera->update(playerCamera->getWindow());	
}

void Player::attack(float deltaTime)
{
	if (glfwGetMouseButton(playerCamera->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)// && tempSpell->getCooldown() <= 0
	{
		createRay();
		spellhandler->createSpell(deltaTime, m_playerPosition, directionVector, spellType);

	}
	spellhandler->spellCooldown(deltaTime);

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
	spellhandler->renderSpell();
}

void Player::setPlayerPos(glm::vec3 pos)
{
	this->m_playerPosition = pos;
}

void Player::spawnPlayer(glm::vec3 pos)
{
	this->m_playerPosition = pos;
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
	return this->m_playerPosition;
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
