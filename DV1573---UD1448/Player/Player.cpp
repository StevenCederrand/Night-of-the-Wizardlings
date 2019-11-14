#include "Pch/Pch.h"
#include "Player.h"
#include <Networking/Client.h>

Player::Player(BulletPhysics* bp, std::string name, glm::vec3 playerPosition, Camera *camera, SpellHandler* spellHandler)
{
	m_playerCamera = camera;
	m_playerPosition = playerPosition;
	m_name = name;
	m_speed = 18.0f;
	m_health = 100;
	m_attackCooldown = 0;
	m_special2Cooldown = 0;
	m_nrOfSpells = 0;
	m_directionVector = glm::vec3(0, 0, 0);
	m_moveDir = glm::vec3(0.0f);
	m_isWalking = false;

	m_spellhandler = spellHandler;
	m_mana = 100.0f; //A  players mana pool
	m_spellType = NORMALATTACK;
	m_specialSpelltype = REFLECT;
	m_specialSpellType2 = ENHANCEATTACK;
	m_specialSpellType3 = FLAMESTRIKE;

	m_bp = bp;
	float temp = 1.0f;
	m_character = m_bp->createCharacter(playerPosition, temp);

	m_client = Client::getInstance();	
}

Player::~Player()
{	
}

void Player::update(float deltaTime)
{																		
	if (m_playerCamera->isCameraActive()) {									// IMPORTANT; DOING THESE WRONG WILL CAUSE INPUT LAG
		m_playerCamera->update();											// Update this first so that subsequent uses are synced
		m_directionVector = glm::normalize(m_playerCamera->getCamFace());	// Update this first so that subsequent uses are synced

		if (!m_logicStop) {
			move(deltaTime);
			attack();
		}
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


	updateListenerProperties();

	/* This is unnecessary*/
	m_attackCooldown -= deltaTime; // Cooldown reduces with time
	m_deflectCooldown -= deltaTime; // Cooldown reduces with time
	m_special2Cooldown -= deltaTime; // Cooldown reduces with time
	m_special3Cooldown -= deltaTime; // Cooldown reduces with time

	//Regenerate mana when we are not deflecting
	if (!m_deflecting && m_mana <= 100 && m_deflectCooldown <= 0) {
		m_mana += 0.5f;
	}
	else if (m_deflectCooldown > 0 && !m_deflecting) {
		m_deflectCooldown -= DeltaTime;
	}

}

void Player::updateListenerProperties()
{
	SoundHandler* shPtr = SoundHandler::getInstance();	

	shPtr->setListenerOrientation(m_playerCamera->getCamFace(),
		m_playerCamera->getCamUp());
	shPtr->setListenerPos(m_playerPosition);	
	shPtr->setSourcePosition(m_playerPosition, BasicAttackSound, m_client->getMyData().guid);
	shPtr->setSourcePosition(m_playerPosition, DeflectSound, m_client->getMyData().guid);
	shPtr->setSourcePosition(m_playerPosition, EnhanceAttackSound, m_client->getMyData().guid);
	shPtr->setSourcePosition(m_playerPosition, StepsSound, m_client->getMyData().guid);
	shPtr->setSourcePosition(m_playerPosition, JumpSound, m_client->getMyData().guid);
	shPtr->setSourceLooping(true, StepsSound, m_client->getMyData().guid);
}

void Player::move(float deltaTime)
{
	SoundHandler* sh = SoundHandler::getInstance();	

	m_frameCount++;
	if (m_frameCount < 5)
		return;

	glm::vec3 lookDirection = m_directionVector;
	lookDirection.y = 0.0f;
	glm::vec3 lookRightVector = m_playerCamera->getCamRight();

	// Move
	m_moveDir = glm::vec3(0.0f);
	if (Input::isKeyHeldDown(GLFW_KEY_A))
	{
		m_moveDir -= lookRightVector;		
		m_isWalking = true;
	}	
	if (Input::isKeyHeldDown(GLFW_KEY_D))
	{
		m_moveDir += lookRightVector;
		m_isWalking = true;
	}
	if (Input::isKeyHeldDown(GLFW_KEY_W))
	{
		m_moveDir += lookDirection;
		m_isWalking = true;
	}
	if (Input::isKeyHeldDown(GLFW_KEY_S))
	{
		m_moveDir -= lookDirection;
		m_isWalking = true;
	}
	// Jump
	if (Input::isKeyHeldDown(GLFW_KEY_SPACE))
		if (m_character->canJump())
		{
			m_character->jump(btVector3(0.0f, 16.0f, 0.0f));
			sh->playSound(JumpSound, m_client->getMyData().guid);
		}

	if (!m_isWalking || !m_character->onGround())
	{
		sh->stopSound(StepsSound, m_client->getMyData().guid);
	}
	else if(m_character->onGround())
	{
		sh->playSound(StepsSound, m_client->getMyData().guid);
	}
	m_isWalking = false;

	// Make sure moving is a constant speed
	if (glm::length(m_moveDir) >= 0.0001f)
		m_moveDir = glm::normalize(m_moveDir);
	
	//update player position
	btScalar yValue = std::ceil(m_character->getLinearVelocity().getY() * 100) / 100;	//Round to two decimals
	btVector3 bulletVec = btVector3(m_moveDir.x * m_speed, -0.01f, m_moveDir.z * m_speed);

	m_character->setVelocityForTimeInterval(bulletVec, deltaTime);

	btVector3 playerPos = m_character->getGhostObject()->getWorldTransform().getOrigin();
	m_playerPosition = glm::vec3(playerPos.getX(), playerPos.getY() + 2.0f, playerPos.getZ());

	m_playerCamera->setCameraPos(m_playerPosition);
	m_character->updateAction(m_bp->getDynamicsWorld(), deltaTime);

}

void Player::attack()
{
	SoundHandler* shPtr = SoundHandler::getInstance();	

	if (Input::isMouseHeldDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		if (m_attackCooldown <= 0)
		{
			m_spellhandler->setSpawnerDirection(m_directionVector);
			m_spellhandler->setSpawnerPosition(m_playerPosition);
			m_attackCooldown = m_spellhandler->createSpell(m_playerPosition, m_directionVector, m_spellType); // Put attack on cooldown
			shPtr->playSound(BasicAttackSound, m_client->getMyData().guid);
		}		
	}

	if (Input::isMouseHeldDown(GLFW_MOUSE_BUTTON_RIGHT) && m_mana > 5)
	{
		if (!m_deflecting)
		{			
			shPtr->playSound(DeflectSound, m_client->getMyData().guid);
			m_deflecting = true;
			m_deflectCooldown = 0.5f; 
		}
		m_mana -= 1;
	}	
	else {
		m_deflecting = false;
	}
	if (Input::isMouseReleased(GLFW_MOUSE_BUTTON_RIGHT)) {
		m_deflecting = false;
	}

	if (Input::isKeyHeldDown(GLFW_KEY_Q))
	{
		if (m_special2Cooldown <= 0)
		{
			if (m_specialSpellType2 == ENHANCEATTACK)
			{
				m_spellhandler->setSpawnerDirection(m_directionVector);
				m_spellhandler->setSpawnerPosition(m_playerPosition);
				// Start loop
				m_enhanceAttack.start();
				
				shPtr->playSound(EnhanceAttackSound, m_client->getMyData().guid);
			}
		}
	}

	if (Input::isKeyHeldDown(GLFW_KEY_R))
	{
		if (m_special3Cooldown <= 0)
		{
			m_spellhandler->setSpawnerDirection(m_directionVector);
			m_spellhandler->setSpawnerPosition(m_playerPosition);
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

	m_character->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
	auto transform = m_character->getGhostObject()->getWorldTransform();
	transform.setOrigin(btVector3(pos.x, pos.y + 1.0f, pos.z));
	m_character->getGhostObject()->setWorldTransform(transform);
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

const float& Player::getAttackCooldown() const
{
	return m_attackCooldown;
}

const float& Player::getSpecialCooldown() const
{
	return m_special2Cooldown;
}

const float& Player::getDeflectCooldown() const
{
	return m_deflectCooldown;
}

const float& Player::getMana() const
{
	return m_mana;
}

const glm::vec3& Player::getPlayerPos() const
{
	return m_playerPosition;
}

const int& Player::getHealth() const
{
	return m_health;
}

Camera* Player::getCamera()
{
	return m_playerCamera;
}

const std::string& Player::getName() const
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
