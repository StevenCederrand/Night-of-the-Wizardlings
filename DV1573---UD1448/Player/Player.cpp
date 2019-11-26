 #include "Pch/Pch.h"
#include "Player.h"
#include <Networking/Client.h>

Player::Player(BulletPhysics* bp, std::string name, glm::vec3 playerPosition, Camera *camera, SpellHandler* spellHandler)
{
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

	m_playerCamera = camera;
	m_playerPosition = playerPosition;
	m_name = name;
	m_speed = 15.2f;
	m_health = 100;
	m_attackCooldown = 0;
	m_specialCooldown = 0;
	m_directionVector = glm::vec3(0, 0, 0);
	m_moveDir = glm::vec3(0.0f);
	m_isWalking = false;
	m_isJumping = false;

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
		move(deltaTime);
		m_playerCamera->update();											// Update this first so that subsequent uses are synced
		m_directionVector = glm::normalize(m_playerCamera->getCamFace());	// Update this first so that subsequent uses are synced

		if (m_playerCamera->isFPEnabled()) {
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
			m_spellhandler->createSpell(m_spellSpawnPosition, m_directionVector, ENHANCEATTACK);
			m_enhanceAttack.attacked();
		}
	}

	if (m_mana > 100) {
		m_mana = 100;
	}
	updateListenerProperties();

	m_attackCooldown -= deltaTime; // Cooldown reduces with time
	m_deflectCooldown -= deltaTime; // Cooldown reduces with time
	m_specialCooldown -= deltaTime; // Cooldown reduces with time
	m_special3Cooldown -= deltaTime; // Cooldown reduces with time

	//Regenerate mana when we are not deflecting
	if (!m_rMouse && m_mana < 100 && m_deflectCooldown <= 0) {
		m_mana += 7.5f * DeltaTime;
	}
	else if (m_deflectCooldown > 0 && !m_rMouse) {
		m_deflectCooldown -= DeltaTime;
	}



	if (m_health <= 0) {
		if (m_firstPersonMesh->getShouldRender() == true) {
			m_firstPersonMesh->setShouldRender(false);
			SoundHandler* shPtr = SoundHandler::getInstance(); //stop Deflect
			shPtr->stopSound(DeflectSound, m_client->getMyData().guid);
		}
	}else{
		if (m_firstPersonMesh->getShouldRender() == false) {
			m_firstPersonMesh->setShouldRender(true);
		}
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
	shPtr->setSourcePosition(m_playerPosition, DeflectSound, m_client->getMyData().guid);
	shPtr->setSourcePosition(m_playerPosition, StepsSound, m_client->getMyData().guid);
	shPtr->setSourcePosition(m_playerPosition, JumpSound, m_client->getMyData().guid);
	shPtr->setSourcePosition(m_playerPosition, LandingSound, m_client->getMyData().guid);
	shPtr->setSourcePosition(m_playerPosition, PickupGraveyardSound);
	shPtr->setSourcePosition(m_playerPosition, PickupMazeSound);
	shPtr->setSourcePosition(m_playerPosition, PickupTunnelsSound);
	shPtr->setSourcePosition(m_playerPosition, PickupTopSound);
	shPtr->setSourcePosition(m_playerPosition, PickupSound);
	shPtr->setSourceLooping(true, StepsSound, m_client->getMyData().guid);

	for (int i = 0; i < NR_OF_SUBSEQUENT_SOUNDS; i++)
	{
		shPtr->setSourcePosition(m_playerPosition, EnhanceAttackSound, m_client->getMyData().guid, i);
	}
}

void Player::move(float deltaTime)
{
	SoundHandler* sh = SoundHandler::getInstance();

	m_frameCount++;
	if (m_frameCount < 5)
		return;

	m_moveDir = glm::vec3(0.0f);

	if (m_playerCamera->isFPEnabled()) {

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

	m_playerPosition = glm::vec3(playerPos.getX(), playerPos.getY(), playerPos.getZ());
	m_playerPosition.y -= characterHalfSize;

	//set cameraPos and spellSpawnPos
	m_cameraPosition = m_playerPosition;
	m_cameraPosition.y += characterHalfSize + characterHalfSize * 0.60f;
	m_spellSpawnPosition = m_playerPosition;
	m_spellSpawnPosition.y += characterHalfSize + characterHalfSize * 0.55f;


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
		//animState.deflecting = false;
	}


	m_firstPersonMesh->update(deltaTime);

}

void Player::attack()
{
	SoundHandler* shPtr = SoundHandler::getInstance();	
	if (Input::isMouseHeldDown(GLFW_MOUSE_BUTTON_RIGHT))
	{
		//Actually deflecting
		if (m_mana > 10) {
			GameObject* shieldObject = new ShieldObject("playerShield");
			shieldObject->loadMesh("ShieldMeshFPS.mesh");

			Transform m_fpsTrans;
			m_fpsTrans.position = m_playerCamera->getCamPos();
			m_fpsTrans.rotation = glm::quat(glm::vec3(glm::radians(m_playerCamera->getPitch()),
				-glm::radians(m_playerCamera->getYaw() + 90.0), 0.0));

			shieldObject->setTransform(m_fpsTrans);
			Renderer::getInstance()->submit(shieldObject, SHIELD);
			if (!m_deflecting) {
				animState.deflecting = true; //Play the animation once
				m_mana -= 10; //This is the initial manacost for the deflect

				shPtr->playSound(DeflectSound, m_client->getMyData().guid);
				m_deflecting = true; //So we don't play sound over and over
			}
			m_mana -= 10.f * DeltaTime;			
			m_deflectCooldown = 0.5f;
		}
		else { //Player is holding down RM without any mana
			
			//Fade out deflect sound
			if (m_deflectSoundGain > 0.0f)
			{
				m_deflectSoundGain -= 2.0f * DeltaTime;
				shPtr->setSourceGain(m_deflectSoundGain, DeflectSound, m_client->getMyData().guid);
			}
			else
			{
				shPtr->stopSound(DeflectSound, m_client->getMyData().guid);				
			}
		}
		m_rMouse = true;
	}
	else if(m_deflecting)
	{		
		//Fade out deflect sound
		if (m_deflectSoundGain > 0.0f)
		{
			m_deflectSoundGain -= 2.0f * DeltaTime;
			shPtr->setSourceGain(m_deflectSoundGain, DeflectSound, m_client->getMyData().guid);
		}
		else
		{
			shPtr->stopSound(DeflectSound, m_client->getMyData().guid);
			m_deflectSoundGain = 0.4f;
			shPtr->setSourceGain(m_deflectSoundGain, DeflectSound, m_client->getMyData().guid);
			m_deflecting = false;
		}		
	}
	else
	{
		if (Input::isMouseHeldDown(GLFW_MOUSE_BUTTON_LEFT))
		{
			if (m_attackCooldown <= 0)
			{

				shPtr->playSound(BasicAttackSound, m_client->getMyData().guid);
				m_attackCooldown = m_spellhandler->createSpell(m_spellSpawnPosition, m_directionVector, NORMALATTACK); // Put attack on cooldown
				animState.casting = true;

			}
		}
		//Special Abilities are to be placed here
		if (Input::isKeyPressed(GLFW_KEY_Q))
		{
			//If we are using the triple spell
			if (m_usingTripleSpell) {
				if (m_specialCooldown <= 0)
				{
					//Sound for enhance spell is handled in spellhandler
					//m_specialCooldown = m_spellhandler->getEnhAttackBase()->m_coolDown;
					/* Because we are swapping spells, we want to keep a more or less uniform cooldown. */
					// Start loop
					m_enhanceAttack.start();
					animState.casTripple = true;
					m_usingTripleSpell = false;
					m_specialCooldown = 3.5f;
				}
			}
			else { //If our active spell is flamestrike
				if (m_specialCooldown <= 0)
				{
					m_spellhandler->createSpell(m_spellSpawnPosition, m_directionVector, FLAMESTRIKE); // Put attack on cooldown

					animState.castPotion = true;
					m_usingTripleSpell = true;
					m_specialCooldown = 3.5f;
				}
			}
		}
	}
	if (Input::isMouseReleased(GLFW_MOUSE_BUTTON_RIGHT)) {
		m_rMouse = false;
		animState.deflecting = false;
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

void Player::onDead()
{
	animState.deflecting = false;
	m_deflecting = false;
}

void Player::onRespawn()
{
	m_mana = 100.0f;
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

const glm::vec3 Player::getMeshHalfSize() const
{
	return glm::vec3(m_bp->getCharacterSize().getX(), m_bp->getCharacterSize().getY(), m_bp->getCharacterSize().getZ());
}

const float& Player::getMana() const
{
	return m_mana;
}

const bool& Player::onGround() const
{
	return m_character->onGround();
}

const bool& Player::usingTripleSpell() const
{
	return m_usingTripleSpell;
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
