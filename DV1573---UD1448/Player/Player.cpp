 #include "Pch/Pch.h"
#include "Player.h"
#include <Networking/Client.h>

Player::Player(std::string name, glm::vec3 playerPosition, Camera *camera, SpellHandler* spellHandler)
{
	// Player first person mesh
	m_firstPersonMesh = new AnimatedObject("fpsMesh");
	m_firstPersonMesh->loadMesh("FPSTESTIGEN.mesh");
	m_firstPersonMesh->initAnimations("CastAnimation", 1.0f, 17.0f);
	m_firstPersonMesh->initAnimations("CastTrippleAnimation", 89.0f, 114.0f);
	m_firstPersonMesh->initAnimations("CastPotionAnimation", 137.0f, 152.0f);
	m_firstPersonMesh->initAnimations("JumpAnimation", 1.0f, 1.0f);
	m_firstPersonMesh->initAnimations("RunAnimation", 116.0f, 136.0f);
	m_firstPersonMesh->initAnimations("IdleAnimation", 19.0f, 87.0f);
	m_firstPersonMesh->initAnimations("DeflectAnimation", 154.0f, 169.0f);
	Renderer::getInstance()->submit(m_firstPersonMesh, ANIMATEDSTATIC);	

	// Player shield
	m_shieldObject = new ShieldObject("playerShield");
	m_shieldObject->loadMesh("ShieldMeshFPS.mesh");
	m_shieldObject->setShouldRender(false);
	Renderer::getInstance()->submit(m_shieldObject, SHIELD);

	// References
	m_playerCamera = camera;
	m_spellhandler = spellHandler;
	m_client = Client::getInstance();
	m_character = BulletPhysics::getInstance()->createCharacter(playerPosition);
	m_character->getGhostObject()->setUserPointer(this);
	m_character->getGhostObject()->setUserIndex(545);

	// Often moving values 
	m_playerPosition = playerPosition;
	m_name = name;
	m_attackCooldown = 0;
	m_specialCooldown = 0;
	m_deflectCooldown = 0;
	m_directionVector = glm::vec3(0, 0, 0);
	m_moveDir = glm::vec3(0.0f);
	m_isWalking = false;
	m_isJumping = false;
	m_mana = 100.0f;
	m_health = 100.0f;

	// Somewhat static values
	m_maxSpeed = 15.2f;
	m_maxMana = 100.0f;
	m_maxHealth = 100.0f;
	m_manaRegen = 100.0f; 

	
	m_maxAttackCooldown = 1.0f;
	m_maxSpecialCooldown = 1.5f;

	m_deflectManaDrain = 10.0f;
	m_specialManaDrain = 30.0f;

	m_mainAtkType = NORMALATTACK;
	m_specialAtkType = ENHANCEATTACK;	

	SoundHandler::getInstance()->setSourceLooping(true, StepsSound, m_client->getMyData().guid);
}

Player::~Player()
{
	delete m_firstPersonMesh;
	delete m_shieldObject;
}

void Player::update(float deltaTime)
{
	//if the game is not in session, always have 100 mana
	if (m_client->getServerState().currentState != NetGlobals::SERVER_STATE::GameInSession)
		m_mana = 100;


	if (m_playerCamera->isCameraActive()) {									// IMPORTANT; DOING THESE WRONG WILL CAUSE INPUT LAG
		move(deltaTime);
		m_playerCamera->update();											// Update this first so that subsequent uses are synced
		m_directionVector = glm::normalize(m_playerCamera->getCamFace());	// Update this first so that subsequent uses are synced

		if (m_playerCamera->isFPEnabled() && !Client::getInstance()->getMyData().health <= 0)
		{
			attack();
		}
	}

	updateMesh();

	if (m_client->isConnectedToSever()) {
		m_client->updatePlayerData(this);

		if (Input::isKeyReleased(GLFW_KEY_F1)) {
			m_client->sendReadyRequestToServer();
		}
	}
	// ENHANCE ATTACK
	if (!m_enhanceAttack.isComplete())
	{
		m_enhanceAttack.update(deltaTime);
		if (m_enhanceAttack.canAttack()) //CAN ATTACK
		{
			m_specialAtkType = m_spellhandler->createSpell(m_spellSpawnPosition, m_directionVector, ENHANCEATTACK);
			m_enhanceAttack.attacked();
		}
	}

	// Sound
	updateListenerProperties();

	// Cooldown reduces with time
	m_attackCooldown -= deltaTime;
	m_specialCooldown -= deltaTime;

	//Regenerate mana when we are not deflecting
	if (!m_deflecting && !m_rMouse && m_mana < m_maxMana)
	{
		m_mana += m_manaRegen * DeltaTime;
		if (m_mana > m_maxMana) 
			m_mana = m_maxMana;
	}


	if (m_health <= 0) 
	{
		if (m_firstPersonMesh->getShouldRender() == true) 
		{
			m_firstPersonMesh->setShouldRender(false);
			SoundHandler* shPtr = SoundHandler::getInstance(); //stop Deflect
			shPtr->stopSound(DeflectSound, m_client->getMyData().guid);
		}
	}
	else
	{
		if (m_firstPersonMesh->getShouldRender() == false) 
			m_firstPersonMesh->setShouldRender(true);
		
		PlayAnimation(deltaTime);
	}

}

void Player::updateListenerProperties()
{
	SoundHandler* shPtr = SoundHandler::getInstance();

	shPtr->setListenerOrientation(m_playerCamera->getCamFace(),
		m_playerCamera->getCamUp());
	shPtr->setListenerPos(m_playerPosition);
	shPtr->setSourcePosition(m_playerPosition, BasicAttackSound, m_client->getMyData().guid);
	shPtr->setSourcePosition(m_playerPosition, BasicAttackSound, m_client->getMyData().guid, 1);		
	shPtr->setSourcePosition(m_playerPosition, StepsSound, m_client->getMyData().guid);
	shPtr->setSourcePosition(m_playerPosition, JumpSound, m_client->getMyData().guid);
	shPtr->setSourcePosition(m_playerPosition, LandingSound, m_client->getMyData().guid);
	shPtr->setSourcePosition(m_playerPosition, PickupGraveyardSound);
	shPtr->setSourcePosition(m_playerPosition, PickupMazeSound);
	shPtr->setSourcePosition(m_playerPosition, PickupTunnelsSound);
	shPtr->setSourcePosition(m_playerPosition, PickupTopSound);
	shPtr->setSourcePosition(m_playerPosition, PickupSound);	

	for (int i = 0; i < NR_OF_SUBSEQUENT_SOUNDS; i++)
	{
		shPtr->setSourcePosition(m_playerPosition, EnhanceAttackSound, m_client->getMyData().guid, i);
		shPtr->setSourcePosition(m_playerPosition, DeflectSound, m_client->getMyData().guid, i);
	}
}

void Player::move(float deltaTime)
{
	SoundHandler* sh = SoundHandler::getInstance();

	m_frameCount++;
	if (m_frameCount < 5)
		return;

	//can't move much in the air
	if (m_character->onGround())
	{
		m_moveDir = glm::vec3(0.0f);
	}


	if (m_playerCamera->isFPEnabled() ) {

		glm::vec3 lookDirection = m_directionVector;
		lookDirection.y = 0.0f;
		glm::vec3 lookRightVector = m_playerCamera->getCamRight();

		// Move
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
		if (Input::isKeyHeldDown(GLFW_KEY_SPACE)) {
			if (m_character->canJump() && !m_isJumping) {
				m_character->jump(btVector3(0.0f, 16.0f, 0.0f));
				animState.jumping = true;
				sh->playSound(JumpSound, m_client->getMyData().guid);
				m_isJumping = true;
			}
		}

		if (m_isJumping && m_character->onGround())
		{
			sh->playSound(LandingSound, m_client->getMyData().guid);
			m_isJumping = false;
			m_oldMoveDir = glm::vec3(0.0f);
		}

		if (!m_isWalking || !m_character->onGround())
		{
			sh->stopSound(StepsSound, m_client->getMyData().guid);
		}
		else if(m_character->onGround())
		{
			m_oldMoveDir = m_moveDir;
			sh->playSound(StepsSound, m_client->getMyData().guid);
		}
		m_isWalking = false;
	}

	//can't move much in the air
	if (!m_character->onGround())
	{
		m_moveDir = m_oldMoveDir + (m_moveDir * 0.5f);
	}

	// Make sure moving is a constant speed
	if (glm::length(m_moveDir) >= 0.0001f)
		m_moveDir = glm::normalize(m_moveDir);

	//update player position
	btScalar yValue = std::ceil(m_character->getLinearVelocity().getY() * 100) / 100;	//Round to two decimals
	btVector3 bulletVec = btVector3(m_moveDir.x * m_maxSpeed, -0.01f, m_moveDir.z * m_maxSpeed);

	m_character->setVelocityForTimeInterval(bulletVec, deltaTime);

	//set playerpos from bullet character
	btVector3 playerPos = m_character->getGhostObject()->getWorldTransform().getOrigin();
	float characterHalfSize = BulletPhysics::getInstance()->getCharacterSize().getY();

	m_playerPosition = glm::vec3(playerPos.getX(), playerPos.getY(), playerPos.getZ());
	m_playerPosition.y -= characterHalfSize;

	//set cameraPos and spellSpawnPos
	m_cameraPosition = m_playerPosition;
	m_cameraPosition.y += characterHalfSize + characterHalfSize * 0.60f;
	m_spellSpawnPosition = m_playerPosition;
	m_spellSpawnPosition.y += characterHalfSize + characterHalfSize * 0.55f;


	m_playerCamera->setCameraPos(m_cameraPosition);
	m_character->updateAction(BulletPhysics::getInstance()->getDynamicsWorld(), deltaTime);
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
	if (animState.castPotion) {
		m_firstPersonMesh->playAnimation("CastPotionAnimation");
		animState.castPotion = false;
	}
	if (animState.casTripple) {
		m_firstPersonMesh->playAnimation("CastTrippleAnimation");
		animState.casTripple = false;
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
		m_firstPersonMesh->playLoopAnimation("DeflectAnimation");
	}

	m_firstPersonMesh->update(deltaTime);
}

void Player::attack()
{
	SoundHandler* shPtr = SoundHandler::getInstance();	

	if (Input::isMouseHeldDown(GLFW_MOUSE_BUTTON_RIGHT))
	{
		// Update the position of the shield mesh
		// Minor optimization only doing it when holding RMB, 
		// technically not good because it causes inconstistencies in framerate
		Transform m_fpsTrans;
		m_fpsTrans.position = m_playerCamera->getCamPos();
		m_fpsTrans.rotation = glm::quat(glm::vec3(glm::radians(m_playerCamera->getPitch()),
			-glm::radians(m_playerCamera->getYaw() + 90.0), 0.0));
		m_shieldObject->setTransform(m_fpsTrans);

		// Tell stuff we're trying to deflect
		animState.deflecting = true;
		m_rMouse = true;

		// Enough mana to cast
		if (m_mana > m_deflectManaDrain) 
		{
			if (!m_deflecting) // Initial cast
			{
				m_deflectSoundGain = shPtr->getMasterVolume(); 
				m_mana -= m_deflectManaDrain; 
				shPtr->playSound(DeflectSound, m_client->getMyData().guid);
				shPtr->setSourceGain(m_deflectSoundGain, DeflectSound, m_client->getMyData().guid);
				m_deflecting = true;
			}
			else // Looping state
			{
				m_mana -= m_deflectManaDrain * DeltaTime;			
			}
		}
		else // No mana but trying to cast
		{ 
			//Fade out deflect sound
			m_deflectSoundGain -= 3.0f * shPtr->getMasterVolume() * DeltaTime;
			if (m_deflectSoundGain > 0.0f)
			{				
				shPtr->setSourceGain(m_deflectSoundGain, DeflectSound, m_client->getMyData().guid);
			}
			else
			{	
				//Stop all sources for this sound. 
				for (int i = 0; i < NR_OF_SUBSEQUENT_SOUNDS; i++)
				{
					shPtr->stopSound(DeflectSound, m_client->getMyData().guid, i);
				}
			}
		}

	}
	else if(m_deflecting) // Not holding RMB but in deflect state
	{					
		m_deflecting = false;
	}
	else if (shPtr->getSourceState(DeflectSound, m_client->getMyData().guid, 0) == AL_PLAYING)
	{
		//Fade out deflect sound
		m_deflectSoundGain -= 3.0f * shPtr->getMasterVolume() * DeltaTime;
		if (m_deflectSoundGain > 0.0f)
		{
			shPtr->setSourceGain(m_deflectSoundGain, DeflectSound, m_client->getMyData().guid);
		}
		else
		{
			m_deflectSoundGain = shPtr->getMasterVolume(); // Will automatically be set to master volume			

			//Stop all sources and set the gain back to max gain. 
			for (int i = 0; i < NR_OF_SUBSEQUENT_SOUNDS; i++)
			{
				shPtr->stopSound(DeflectSound, m_client->getMyData().guid, i);
				shPtr->setSourceGain(m_deflectSoundGain,
					DeflectSound, m_client->getMyData().guid, i);
			}
		}
	}

	if (!m_deflecting && Input::isMouseHeldDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		if (m_attackCooldown <= 0.0f)
		{
			m_spellhandler->createSpell(m_spellSpawnPosition, m_directionVector, NORMALATTACK); // Put attack on cooldown
			m_attackCooldown = m_maxAttackCooldown;
			animState.casting = true;
			shPtr->playSound(BasicAttackSound, m_client->getMyData().guid);
		}
	}

	//Special Abilities are to be placed here
	if (!m_deflecting && Input::isKeyPressed(GLFW_KEY_Q))
	{
		if (m_specialAtkType == ENHANCEATTACK)
		{
			if (m_specialCooldown <= 0.0f && m_mana >= m_specialManaDrain)
			{
				m_enhanceAttack.start();
				m_specialCooldown = m_maxSpecialCooldown;
				m_mana -= m_specialManaDrain;
				animState.casTripple = true;

				m_usingTripleSpell = false;
			}
		}
		else
		{
			if (m_specialCooldown <= 0.0f && m_mana >= m_specialManaDrain)
			{
				// Randomize spell
				m_specialAtkType = m_spellhandler->createSpell(m_spellSpawnPosition, m_directionVector, m_specialAtkType);
				m_specialCooldown = m_maxSpecialCooldown;
				m_mana -= m_specialManaDrain;
				animState.castPotion = true;

				m_usingTripleSpell = true;
			}
		}
	}

	if (Input::isMouseReleased(GLFW_MOUSE_BUTTON_RIGHT)) {
		animState.deflecting = false;
		m_rMouse = false;	
		m_deflecting = false;
	}

	// Update our shield for the renderer
	m_shieldObject->setShouldRender(m_deflecting);
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
	m_fpsTrans.rotation = glm::quat(glm::vec3(
		glm::radians(m_playerCamera->getPitch()),
		-glm::radians(m_playerCamera->getYaw() + 90.0f),
		0.0f));

	m_firstPersonMesh->setTransform(m_fpsTrans);

}

void Player::onDead()
{
	animState.deflecting = false;
	m_deflecting = false;
}

void Player::onRespawn()
{
	m_mana = 100.0f;
}

void Player::increaseMana(const float& increase)
{
	m_mana += increase;
	
	if (m_mana > m_maxMana) {
		// Clamp mana
		m_mana = m_maxMana;
	}
}

void Player::setHealth(int health)
{
	m_health = health;
}

void Player::setMana(int mana)
{
	m_mana = mana;
}

void Player::setSpeed(float speed)
{
	m_maxSpeed = speed;
}

void Player::logicStop(const bool& stop)
{
	m_logicStop = stop;
}

void Player::submitHudHandler(HudHandler* hudHandler)
{
	m_hudHandler = hudHandler;
}

const float& Player::getAttackCooldown() const
{
	return m_attackCooldown;
}

const float& Player::getSpecialCooldown() const
{
	return m_specialCooldown;
}

const float& Player::getMaxAttackCooldown() const
{
	return m_maxAttackCooldown;
}

const float& Player::getMaxSpecialCooldown() const
{
	return m_maxSpecialCooldown;
}

const glm::vec3 Player::getMeshHalfSize() const
{
	return glm::vec3(BulletPhysics::getInstance()->getCharacterSize().getX(), BulletPhysics::getInstance()->getCharacterSize().getY(), BulletPhysics::getInstance()->getCharacterSize().getZ());
}

const float& Player::getMana() const
{
	return m_mana;
}

const bool& Player::onGround() const
{
	return m_character->onGround();
}

const OBJECT_TYPE& Player::currentSpell() const
{
	return m_specialAtkType;
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
