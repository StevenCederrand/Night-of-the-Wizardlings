 #include "Pch/Pch.h"
#include "Player.h"
#include <Networking/Client.h>

Player::Player(BulletPhysics* bp, std::string name, glm::vec3 playerPosition, Camera *camera, SpellHandler* spellHandler)
{
	m_firstPersonMesh = new AnimatedObject("fpsMesh");
	m_firstPersonMesh->loadMesh("Fps2Arm.mesh");
	m_firstPersonMesh->initAnimations("CastAnimation", 1.0f, 20.0f);
	m_firstPersonMesh->initAnimations("JumpAnimation", 21.0f, 35.0f);
	m_firstPersonMesh->initAnimations("RunAnimation", 36.0f, 55.0f);
	m_firstPersonMesh->initAnimations("IdleAnimation", 56.0f, 125.0f);
	m_firstPersonMesh->initAnimations("DeflectAnimation", 126.0f, 175.0f);



	Renderer::getInstance()->submit(m_firstPersonMesh, ANIMATEDSTATIC);

	m_playerCamera = camera;
	m_playerPosition = playerPosition;
	m_name = name;
	m_speed = 18.0f;
	m_health = 100;
	m_attackCooldown = 0;
	m_specialCooldown = 0;
	m_directionVector = glm::vec3(0, 0, 0);
	m_moveDir = glm::vec3(0.0f);

	m_spellhandler = spellHandler;
	m_mana = 100.0f; //A  players mana pool

	m_maxAttackCooldown = m_spellhandler->getAttackBase()->m_coolDown;
	m_maxSpecialCooldown = m_spellhandler->getEnhAttackBase()->m_coolDown;
	m_bp = bp;
	float temp = 1.0f;
	m_character = m_bp->createCharacter(playerPosition, temp);

	m_client = Client::getInstance();
}

Player::~Player()
{
	delete m_firstPersonMesh;
}

void Player::update(float deltaTime)
{																		
	if (m_playerCamera->isCameraActive()) {									// IMPORTANT; DOING THESE WRONG WILL CAUSE INPUT LAG
		m_playerCamera->update();											// Update this first so that subsequent uses are synced
		m_directionVector = glm::normalize(m_playerCamera->getCamFace());	// Update this first so that subsequent uses are synced

		move(deltaTime);
		
		if (!m_logicStop) {
			attack();
		}
	}

	updateMesh();

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
			m_spellhandler->createSpell(m_spellSpawnPosition, m_directionVector, ENHANCEATTACK);
			m_enhanceAttack.attacked();
		}
	}

	m_attackCooldown -= deltaTime; // Cooldown reduces with time
	m_deflectCooldown -= deltaTime; // Cooldown reduces with time
	m_specialCooldown -= deltaTime; // Cooldown reduces with time
	m_special3Cooldown -= deltaTime; // Cooldown reduces with time
	
	//Regenerate mana when we are not deflecting
	if (!m_rMouse && m_mana <= 100 && m_deflectCooldown <= 0) {
		m_mana += 0.1f;
	}
	else if (m_deflectCooldown > 0 && !m_rMouse) {
		m_deflectCooldown -= DeltaTime;
	}
	PlayAnimation(deltaTime);

}

void Player::move(float deltaTime)
{
	m_frameCount++;
	if (m_frameCount < 5)
		return;
	
	m_moveDir = glm::vec3(0.0f);

	if (!m_logicStop) {

		glm::vec3 lookDirection = m_directionVector;
		lookDirection.y = 0.0f;
		glm::vec3 lookRightVector = m_playerCamera->getCamRight();

		// Move
		if (Input::isKeyHeldDown(GLFW_KEY_A))
			m_moveDir -= lookRightVector;
		if (Input::isKeyHeldDown(GLFW_KEY_D))
			m_moveDir += lookRightVector;
		if (Input::isKeyHeldDown(GLFW_KEY_W))
			m_moveDir += lookDirection;
		if (Input::isKeyHeldDown(GLFW_KEY_S))
			m_moveDir -= lookDirection;

		// Jump
		if (Input::isKeyHeldDown(GLFW_KEY_SPACE))
			if (m_character->canJump())
				m_character->jump(btVector3(0.0f, 16.0f, 0.0f));

	}
	// Make sure moving is a constant speed
	if (glm::length(m_moveDir) >= 0.0001f)
		m_moveDir = glm::normalize(m_moveDir);
	
	//update player position
	btScalar yValue = std::ceil(m_character->getLinearVelocity().getY() * 100) / 100;	//Round to two decimals
	btVector3 bulletVec = btVector3(m_moveDir.x * m_speed, -0.01f, m_moveDir.z * m_speed);

	m_character->setVelocityForTimeInterval(bulletVec, deltaTime);

	//set playerpos from bullet character
	btVector3 playerPos = m_character->getGhostObject()->getWorldTransform().getOrigin();
	float characterHalfSize = m_bp->getCharacterSize().getY();

	m_playerPosition = glm::vec3(playerPos.getX(), playerPos.getY()-1.0f, playerPos.getZ());
	//m_playerPosition.y -= characterHalfSize;

	//set cameraPos and spellSpawnPos 
	m_cameraPosition = m_playerPosition;
	m_cameraPosition.y += characterHalfSize + characterHalfSize*0.85f;
	m_spellSpawnPosition = m_playerPosition;
	m_spellSpawnPosition.y += (2 * characterHalfSize) * 0.85f;


	m_playerCamera->setCameraPos(m_cameraPosition);
	m_character->updateAction(m_bp->getDynamicsWorld(), deltaTime);
}

void Player::PlayAnimation(float deltaTime)
{
	

	if (animState.running){
		m_firstPersonMesh->playLoopAnimation("RunAnimation");
		animState.running = false;
	}
	if (animState.casting) {
		m_firstPersonMesh->playAnimation("CastAnimation");
		animState.casting = false;
	}
	if (animState.jumping) {
		m_firstPersonMesh->playAnimation("JumpAnimation");
		animState.jumping = false;
	}
	if (animState.idle){
		m_firstPersonMesh->playLoopAnimation("IdleAnimation");
		animState.idle = false;
	}
	if (animState.deflecting)
	{
		m_firstPersonMesh->playAnimation("DeflectAnimation");
		animState.deflecting = false;
	}

	m_firstPersonMesh->update(deltaTime);

}



void Player::attack()
{
	if (Input::isMouseHeldDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		if (m_attackCooldown <= 0)
		{
			m_attackCooldown = m_spellhandler->createSpell(m_spellSpawnPosition, m_directionVector, NORMALATTACK); // Put attack on cooldown
			animState.casting = true;

		}
	}

	if (Input::isMouseHeldDown(GLFW_MOUSE_BUTTON_RIGHT))
	{

		//Actually deflecting
		if (m_mana > 10) {
			if (!m_deflecting) {
				animState.deflecting = true; //Play the animation once
				m_mana -= 10; //This is the initial manacost for the deflect
			}
			m_mana -= 1;
			m_deflecting = true;
			m_deflectCooldown = 0.5f; 			
		}
		else { //Player is holding down RM without any mana
			m_deflecting = false;
		}
		m_rMouse = true;
	}
	if (Input::isMouseReleased(GLFW_MOUSE_BUTTON_RIGHT)) {
		m_deflecting = false;
		m_rMouse = false;
	}

	if (Input::isKeyHeldDown(GLFW_KEY_Q))
	{
		if (m_specialCooldown <= 0)
		{
			m_specialCooldown = m_spellhandler->getEnhAttackBase()->m_coolDown;
			// Start loop
			m_enhanceAttack.start();
			animState.casting = true;
		}
	}

	if (Input::isKeyHeldDown(GLFW_KEY_R))
	{
		if (m_special3Cooldown <= 0)
		{
			m_special3Cooldown = m_spellhandler->createSpell(m_spellSpawnPosition, m_directionVector, FLAMESTRIKE); // Put attack on cooldown

			animState.casting = true;
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

void Player::updateMesh()
{
	btVector3 velocity = m_character->getLinearVelocity();
	float speed = glm::length(glm::vec3(velocity.getX(), 0.0f, velocity.getZ()));

	if (animState.casting == false)
	{
		if (speed > 0)
			animState.running = true;
		else
			animState.idle = true;
	}

	Transform m_fpsTrans;

	m_fpsTrans.position = m_playerCamera->getCamPos();
	//m_fpsTrans.position = glm::vec3(0.0f, 0.0f, 0.0f);
	m_fpsTrans.rotation = glm::quat(glm::vec3(
		glm::radians(m_playerCamera->getPitch()),
		-glm::radians(m_playerCamera->getYaw() + 90.0f),
		0.0f));
	//m_fpsTrans.scale = glm::vec3(10.0f, 10.0f, 10.0f);

	m_firstPersonMesh->setTransform(m_fpsTrans);

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
	return m_specialCooldown;
}

const float& Player::getDeflectCooldown() const
{
	return m_deflectCooldown;
}

const float& Player::getMaxAttackCooldown() const
{
	return m_maxAttackCooldown;
}

const float& Player::getMaxSpecialCooldown() const
{
	return m_maxSpecialCooldown;
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

const AnimationState* Player::getAnimState() const
{
	return &animState;
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
