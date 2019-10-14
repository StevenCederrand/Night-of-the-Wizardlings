#include "Pch/Pch.h"
#include "Player.h"
#include <Networking/Client.h>



Player::Player(BulletPhysics* bp, std::string name, glm::vec3 playerPosition, Camera *camera)
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
	m_nrOfSpells = 0;
	m_directionVector = glm::vec3(0, 0, 0);
	m_moveDir = glm::vec3(0.0f);
	m_spellType = NORMALATTACK;
	spellhandler = new SpellHandler(playerPosition, m_directionVector);


	btConvexShape* playerShape = new btCapsuleShape(0.25, 1);
	btPairCachingGhostObject* ghostObject = new btPairCachingGhostObject();


	ghostObject->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 20, 0)));
	/*btTransform startTransform;
	startTransform.setIdentity();*/
	//startTransform.setOrigin(btVector3(m_playerPosition.x, m_playerPosition.y, m_playerPosition.z));
	//ghostObject->setWorldTransform(startTransform);
	bp->getDynamicsWorld()->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	ghostObject->setCollisionShape(playerShape);
	ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	controller = new btKinematicCharacterController(ghostObject, playerShape, 0.5f, btVector3(0.0f, 1.0f, 0.0f));
	bp->getDynamicsWorld()->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
	bp->getDynamicsWorld()->addAction(controller);
	controller->setGravity(btVector3(0.0f, -9.0f, 0.0f));
	controller->setMaxPenetrationDepth(0.1f);
	controller->setUp(btVector3(0.0f, 1.0f, 0.0f));
	//logTrace(controller->getUp().getY());
	//controller->jump();
	m_bp = bp;

	/*btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, myMotionState, playerShape, btVector3(0.0f, 0.0f, 0.0f));
	m_body = new btRigidBody(rbInfo);

	bp->getDynamicsWorld()->addRigidBody(m_body);*/
	m_frameCount = 0;
}

Player::~Player()
{
	delete m_playerCamera;
	delete spellhandler;
}

void Player::update(float deltaTime)
{
	controller->updateAction(m_bp->getDynamicsWorld(), deltaTime);
	move(deltaTime);
	spellhandler->spellUpdate(deltaTime);
	attack(deltaTime);
}


void Player::move(float deltaTime)
{
	m_frameCount++;
	if (m_frameCount < 2)
	{
		return;
	}
	glm::vec3 camFace = m_playerCamera->getCamFace();
	glm::vec3 camRight = m_playerCamera->getCamRight();
	float xspeed = 1.0f;

	//camFace.y = 0;
	//auto& totalForce = m_body->getLinearVelocity();
	btVector3 totalForce = controller->getLinearVelocity();
	
	m_moveDir = glm::vec3(0.0f);

	//m_body->activate();
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

	if (glm::length(m_moveDir) >= 0.01f)
		m_moveDir = glm::normalize(m_moveDir);

	m_playerPosition += m_moveDir * m_speed * deltaTime;
	setPlayerPos(m_playerPosition);
	m_playerCamera->setCameraPos(m_playerPosition);
	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		//controller->jump(btVector3(0, 10, 0));		
		logTrace(controller->canJump());
		if (controller->canJump())
		{			
			//controller->setLinearVelocity(btVector3(totalForce.getX(), totalForce.getY(), 100.0f));
			//controller->jump();
		}	

				
	}	


	//move the physics box
	btScalar y =  controller->getLinearVelocity().getY();
	//logTrace(y);
	int yint = y;
	y = yint / 10000;
	//btScalar yValue = controller->getGhostObject()->getWorldTransform().getOrigin().getY();
	btVector3 translate; //= btVector3(0.0f, 0.0f, 0.0f);
	translate = btVector3(m_moveDir.x * m_speed * deltaTime*xspeed, 
		y,
		m_moveDir.z * m_speed * deltaTime*xspeed);
	//m_body->setLinearVelocity(translate);

	btScalar h = controller->getLinearVelocity().getY();

	/*logTrace("h ");
	logTrace(h);
	logTrace(" ");
	logTrace(translate.getX());
	logTrace(translate.getY());
	logTrace(translate.getZ());*/

	controller->setLinearVelocity(translate);

	//character controller
	btVector3 playerPos = controller->getGhostObject()->getWorldTransform().getOrigin();
	m_playerPosition = glm::vec3(playerPos.getX(), playerPos.getY()* 2, playerPos.getZ());
	//setPlayerPos(m_playerPosition);
	m_playerCamera->setCameraPos(m_playerPosition);
	m_playerCamera->update(m_playerCamera->getWindow());
	//m_body->getWorldTransform().setOrigin(playerPos);

	
	/*logTrace(playerPos.getX());
	logTrace(playerPos.getY());
	logTrace(playerPos.getZ());*/
	//change playerPos based on the physics box 
	//btVector3 playerPos = m_body->getCenterOfMassPosition();
	//
	//playerPosition = glm::vec3(playerPos.getX(), playerPos.getY()*2, playerPos.getZ());
	//setPlayerPos(playerPosition);
	//playerCamera->setCameraPos(playerPosition);
	//playerCamera->update(playerCamera->getWindow());

	//inputVector = moveDir;
	//playerPosition += inputVector * speed * deltaTime;
	//setPlayerPos(playerPosition);
	//playerCamera->setCameraPos(playerPosition);
	//playerCamera->update(playerCamera->getWindow());
}

void Player::attack(float deltaTime)
{
	if (glfwGetMouseButton(m_playerCamera->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		createRay();
		spellhandler->setType(m_spellType);
		if (spellhandler->createSpell(deltaTime, m_playerPosition, m_directionVector, spellhandler->getType()))
		{
			m_spellType = NORMALATTACK;
		}
	}

	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_2) == GLFW_PRESS) //&& spellhandler->isSpellReadyToCast(ENHANCEATTACK) == true)
	{
		m_spellType = ENHANCEATTACK;
	}

	if (m_spellType == ENHANCEATTACK)
	{
		createRay();
		spellhandler->setType(m_spellType);
		if (spellhandler->createSpell(deltaTime, m_playerPosition, m_directionVector, spellhandler->getType()))
		{
			m_spellType = NORMALATTACK;
		}
	}

	if (glfwGetKey(m_playerCamera->getWindow(), GLFW_KEY_3) == GLFW_PRESS) //&& spellhandler->isSpellReadyToCast(FLAMESTRIKE) == true)
	{
		createRay();
		spellhandler->setType(FLAMESTRIKE);
		if (spellhandler->createSpell(deltaTime, m_playerPosition, m_directionVector, spellhandler->getType()))
		{
			m_spellType = NORMALATTACK;
		}
	}


	spellhandler->spellCooldown(deltaTime);
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

void Player::renderSpell()
{
	Client::getInstance()->updatePlayerData(this);

	spellhandler->renderSpell();

}

void Player::setPlayerPos(glm::vec3 pos)
{
	m_playerPosition = pos;
}

void Player::spawnPlayer(glm::vec3 pos)
{
	m_playerPosition = pos;
}

void Player::createRigidBody(BulletPhysics* bp)
{
	CollisionObject object = capsule;

	m_body = bp->createObject(object, 10.0f, m_playerPosition, glm::vec3(1.0f, 3.0f,1.0f));
	m_body->setUserPointer(this);
}

void Player::forceUp()
{
	m_body->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
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
