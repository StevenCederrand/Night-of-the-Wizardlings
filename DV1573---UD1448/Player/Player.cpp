#include "Pch/Pch.h"
#include "Player.h"
#include <Networking/Client.h>

Player::Player(BulletPhysics* bp, std::string name, glm::vec3 playerPosition, Camera *camera, SpellHandler* spellHandler)
{
	m_playerCamera = camera;
	m_playerPosition = playerPosition;
	m_name = name;
	m_speed = 5;
	m_health = 100;
	m_attackCooldown = 0;
	m_special2Cooldown = 0;
	m_nrOfSpells = 0;
	m_directionVector = glm::vec3(0, 0, 0);
	m_moveDir = glm::vec3(0.0f);

	m_spellhandler = spellHandler;
	m_spellType = NORMALATTACK;
	m_specialSpelltype = REFLECT;
	m_specialSpellType2 = ENHANCEATTACK;
	m_specialSpellType3 = FLAMESTRIKE;

	m_bp = bp;
	m_character = m_bp->createCharacter(playerPosition.y);

	m_client = Client::getInstance();
}

Player::~Player()
{
}

void Player::update(float deltaTime)
{																		
																		// IMPORTANT; DOING THESE WRONG WILL CAUSE INPUT LAG
	m_playerCamera->update(m_playerCamera->getWindow());				// Update this first so that subsequent uses are synced
	m_directionVector = glm::normalize(m_playerCamera->getCamFace());	// Update this first so that subsequent uses are synced
	move(deltaTime);													// Update this first so that subsequent uses are synced
	m_character->updateAction(m_bp->getDynamicsWorld(), deltaTime);
	if (!m_logicStop) {
		move(deltaTime);
		attack();
	}
	
	if (m_client->isConnectedToSever()) {
		m_client->updatePlayerData(this);
	}
	if (Input::isKeyReleased(GLFW_KEY_E)) {
		m_client->sendStartRequestToServer();
	}

	// ENHANCE ATTACK
	if (!m_enhanceAttack.isComplete())
	{
		m_enhanceAttack.update(deltaTime);
		if (m_enhanceAttack.canAttack()) //CAN ATTACK
		{
			m_spellhandler->createSpell(m_playerPosition, m_directionVector, ENHANCEATTACK);
			m_enhanceAttack.attacked();
		}
		if (m_enhanceAttack.isComplete()) //DONE
		{
			m_special2Cooldown = m_enhanceAttack.getCooldown(); // GET from enhance attack handler
		}
	}

	m_spellhandler->setSpawnerDirection(m_directionVector);
	m_spellhandler->setSpawnerPosition(m_playerPosition);

	m_attackCooldown -= deltaTime; // Cooldown reduces with time
	m_specialCooldown -= deltaTime; // Cooldown reduces with time
	m_special2Cooldown -= deltaTime; // Cooldown reduces with time
	m_special3Cooldown -= deltaTime; // Cooldown reduces with time

	m_timeLeftInDeflectState -= deltaTime;

	if (m_timeLeftInDeflectState < 0.0f) {
		m_deflecting = false;
		m_timeLeftInDeflectState = 0.0f;
	}
}

void Player::move(float deltaTime)
{
	m_frameCount++;
	if (m_frameCount < 5)
		return;

	glm::vec3 lookDirection = m_directionVector;
	lookDirection.y = 0.0f;
	glm::vec3 lookRightVector = m_playerCamera->getCamRight();

	// Move
	m_moveDir = glm::vec3(0.0f);
	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
		m_moveDir -= lookRightVector;
	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
		m_moveDir += lookRightVector;
	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
		m_moveDir += lookDirection;
	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
		m_moveDir -= lookDirection;

	// Jump
	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
		if (m_character->canJump())
			m_character->jump(btVector3(0.0f, 3.0f, 0.0f));

	// Make sure moving is a constant speed
	if (glm::length(m_moveDir) >= 0.0001f)
		m_moveDir = glm::normalize(m_moveDir);
	
	//update player position
	btScalar yValue = std::ceil(m_character->getLinearVelocity().getY() * 100.0) / 100.0;	//Round to two decimals
	btVector3 translate = btVector3(m_moveDir.x * m_speed * deltaTime, yValue, m_moveDir.z * m_speed * deltaTime);
	m_character->setLinearVelocity(translate);
	
	//update playercamera position
	btVector3 playerPos = m_character->getGhostObject()->getWorldTransform().getOrigin();
	m_playerPosition = glm::vec3(playerPos.getX(), playerPos.getY() * 1.5, playerPos.getZ());
	
	m_playerCamera->setCameraPos(m_playerPosition);
}

void Player::attack()
{
	if (glfwGetMouseButton(m_playerCamera->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (m_attackCooldown <= 0)
		{
			m_attackCooldown = m_spellhandler->createSpell(m_playerPosition, m_directionVector, m_spellType); // Put attack on cooldown
		}
	}

	if (glfwGetMouseButton(m_playerCamera->getWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if (m_specialCooldown <= 0)
		{
			m_specialCooldown = m_spellhandler->getReflectBase()->m_coolDown;
			m_timeLeftInDeflectState = m_spellhandler->getReflectBase()->m_lifeTime;
			m_deflecting = true;
		}
	}

	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (m_special2Cooldown <= 0)
		{
			if (m_specialSpellType2 == ENHANCEATTACK)
			{
				// Start loop
				m_enhanceAttack.start();
			}
		}
	}

	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_E) == GLFW_PRESS)
	{
		if (m_special3Cooldown <= 0)
		{
			m_special3Cooldown = m_spellhandler->createSpell(m_playerPosition, m_directionVector, m_specialSpellType3); // Put attack on cooldown
		}
	}
}

void Player::createRay()
{
	float x = (2.0f * static_cast<float>(m_playerCamera->getXpos())) / SCREEN_WIDTH - 1.0f;
	float y = 1.0f - (2.0f * static_cast<float>(m_playerCamera->getYpos())) / SCREEN_HEIGHT;
	float z = 1.0f;

	//-----Spaces-----//
	glm::vec3 rayNDC = glm::vec3(x, y, z);
	glm::vec4 rayClip = glm::vec4(rayNDC.x, rayNDC.y, -1.0f, 1.0f);
	glm::vec4 rayEye = inverse(m_playerCamera->getProjMat()) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
	glm::vec4 rayWorldTemp = glm::vec4(inverse(m_playerCamera->getViewMat()) * rayEye);

	m_directionVector = normalize(glm::vec3(rayWorldTemp.x, rayWorldTemp.y, rayWorldTemp.z));
}

void Player::setPlayerPos(glm::vec3 pos)
{
	this->m_playerPosition = pos;
}

void Player::spawnPlayer(glm::vec3 pos)
{
	this->m_playerPosition = pos;
}

void Player::setHealth(int health)
{
	m_health = health;
}

void Player::setSpeed(float speed)
{
	m_speed = speed;
}

void Player::logicStop(const bool& stop)
{
	m_logicStop = stop;
}

glm::vec3 Player::getPlayerPos() const
{
	return m_playerPosition;
}

int Player::getHealth() const
{
	return m_health;
}

Camera* Player::getCamera()
{
	return m_playerCamera;
}

std::string Player::getName() const
{
	return m_name;
}

const bool& Player::isDeflecting() const
{
	return m_deflecting;
}

bool Player::isDead()
{
	if (m_health <= 0)
	{
		return true;
	}
	else
		return false;
}
