#include "Pch/Pch.h"
#include "Player.h"
#include <Networking/Client.h>



Player::Player(BulletPhysics* bp, std::string name, glm::vec3 playerPosition, Camera *camera, SpellHandler* spellHandler)
{
	if (camera == NULL) {
		 m_playerCamera = new Camera();
	}
	m_playerCamera = camera;
	m_playerPosition = playerPosition;
	m_name = name;
	m_speed = 5;
	m_health = 100;
	m_attackCooldown = 0;
	m_specialCooldown = 0;
	m_nrOfSpells = 0;
	m_directionVector = glm::vec3(0, 0, 0);
	m_moveDir = glm::vec3(0.0f);
	m_spellType = ENHANCEATTACK;

	//m_frameCount = 0;
	//tempSpell = new AttackSpell("Spell", playerPosition, directionVector, 50, 2, "TestSphere.mesh");
	
	m_spellhandler = spellHandler;

	m_bp = bp;
	m_character = m_bp->createCharacter();
}

Player::~Player()
{
	delete m_playerCamera;
}

void Player::update(float deltaTime)
{
	m_character->updateAction(m_bp->getDynamicsWorld(), deltaTime);
	move(deltaTime);
	attack();
	Client* client = Client::getInstance();
	client->updatePlayerData(this);



	if (Input::isKeyReleased(GLFW_KEY_E)) {
		client->sendStartRequestToServer();
	}


	// ENHANCE ATTACK
	if (!m_enhanceAttack.isComplete())
	{
		m_enhanceAttack.update(deltaTime);
		if (m_enhanceAttack.canAttack()) //CAN ATTACK
		{
			createRay();
			m_spellhandler->createSpell(m_playerPosition, m_directionVector, ENHANCEATTACK);
			m_enhanceAttack.attacked();
			// Start loop
		}
		if (m_enhanceAttack.isComplete()) //DONE
		{
			m_specialCooldown = m_enhanceAttack.getCooldown(); // GET from enhance attack
		}
	}
}


void Player::move(float deltaTime)
{
	m_frameCount++;
	if (m_frameCount < 5)
	{
		return;
	}
	glm::vec3 camFace = m_playerCamera->getCamFace();
	glm::vec3 camRight = m_playerCamera->getCamRight();

	btVector3 totalForce = m_character->getLinearVelocity();
	
	m_moveDir = glm::vec3(0.0f);

	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
	{
		m_moveDir -= camRight;
	}
	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
	{
		m_moveDir += camRight;
	}
	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
	{
		m_moveDir += camFace;
	}
	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
	{
		m_moveDir -= camFace;
	}

	if (glm::length(m_moveDir) >= 0.0001f)
		m_moveDir = glm::normalize(m_moveDir);

	m_playerPosition += m_moveDir * m_speed * deltaTime;
	setPlayerPos(m_playerPosition);
	m_playerCamera->setCameraPos(m_playerPosition);

	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{		
		if (m_character->canJump())
		{			
			m_character->jump(btVector3(0.0f, 3.0f, 0.0f));
		}					
	}	


	btScalar yValue = std::ceil(m_character->getLinearVelocity().getY()*100.0) / 100.0;	//Round to two decimals
	btVector3 translate = btVector3
		(m_moveDir.x * m_speed * deltaTime,
		yValue,
		m_moveDir.z * m_speed * deltaTime);
	m_character->setLinearVelocity(translate);

	//update playercamera position
	btVector3 playerPos = m_character->getGhostObject()->getWorldTransform().getOrigin();
	m_playerPosition = glm::vec3(playerPos.getX(), playerPos.getY()* 2, playerPos.getZ());
	
	m_playerCamera->setCameraPos(m_playerPosition);
	m_playerCamera->update(m_playerCamera->getWindow());


	m_attackCooldown -= deltaTime; // Cooldown reduces with time
	m_specialCooldown -= deltaTime; // Cooldown reduces with time
}

void Player::attack()
{
	if (glfwGetMouseButton(m_playerCamera->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (m_attackCooldown <= 0)
		{
			createRay();
			m_spellhandler->createSpell(m_playerPosition, m_directionVector, NORMALATTACK);
			m_attackCooldown = m_spellhandler->getSpellBase(NORMALATTACK).m_coolDown; // Put attack on cooldown
		}
	}

	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (m_specialCooldown <= 0)
		{
			if (m_spellType == ENHANCEATTACK)
			{
				// Start loop
				m_enhanceAttack.start();
			}
			//createRay();
			//m_spellType = ENHANCEHANDLER;
			//m_spellhandler->createSpell(m_playerPosition, m_directionVector, m_spellType);
			//m_specialCooldown = m_spellhandler->getSpellBase(m_spellType).m_coolDown; // Put attack on cooldown
		}
	}


	//if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_3) == GLFW_PRESS) //&& spellhandler->isSpellReadyToCast(FLAMESTRIKE) == true)
	//{
	//	createRay();
	//	spellhandler->setType(FLAMESTRIKE);
	//	if (spellhandler->createSpell(deltaTime, m_playerPosition, m_directionVector, spellhandler->getType()))
	//	{
	//		m_spellType = NORMALATTACK;
	//	}
	//}
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

bool Player::isDead()
{
	if (m_health <= 0)
	{
		return true;
	}
	else
		return false;
}
