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
	tempSpell = new AttackSpell("Spell", playerPosition, directionVector, 50, 2, "TestSphere.mesh");


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
	frameCount = 0;
}

Player::~Player()
{
	delete playerCamera;
	delete tempSpell;
}

void Player::update(float deltaTime)
{
	controller->updateAction(m_bp->getDynamicsWorld(), deltaTime);
	move(deltaTime);
	attack(deltaTime);
	updateAttack(deltaTime);	
}

void Player::move(float deltaTime)
{
	frameCount++;
	if (frameCount < 2)
	{
		return;
	}
	glm::vec3 camFace = playerCamera->getCamFace();
	glm::vec3 camRight = playerCamera->getCamRight();
	float xspeed = 1.0f;

	//camFace.y = 0;
	//auto& totalForce = m_body->getLinearVelocity();
	btVector3 totalForce = controller->getLinearVelocity();
	
	moveDir = glm::vec3(0.0f);

	//m_body->activate();
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

	if (glfwGetKey(playerCamera->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
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
	translate = btVector3(moveDir.x * speed * deltaTime*xspeed, 
		y,
		moveDir.z * speed * deltaTime*xspeed);
	//m_body->setLinearVelocity(translate);

	btScalar h = controller->getLinearVelocity().getY();
	
	controller->setLinearVelocity(translate);

	//character controller
	btVector3 playerPos = controller->getGhostObject()->getWorldTransform().getOrigin();
	m_playerPosition = glm::vec3(playerPos.getX(), playerPos.getY()* 2, playerPos.getZ());
	//setPlayerPos(m_playerPosition);
	playerCamera->setCameraPos(m_playerPosition);
	playerCamera->update(playerCamera->getWindow());
	//m_body->getWorldTransform().setOrigin(playerPos);
}

void Player::attack(float deltaTime)
{
	if (glfwGetMouseButton(playerCamera->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && attackCooldown <= 0)
	{
		createRay();
		AttackSpell tempSpell2 = *tempSpell;
		//tempSpell2.setSpellPos(glm::vec3(0.0f) + directionVector);
		tempSpell2.setSpellPos(glm::vec3(m_playerPosition.x, m_playerPosition.y - 1.8f, m_playerPosition.z) + directionVector); //-1.8 = spwn point for spell, spell need to be 0 and playerPos is set to (0,1.8,0)
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
		//Renderer::getInstance()->render(object, 0);
	}
}

void Player::setPlayerPos(glm::vec3 pos)
{
	this->m_playerPosition = pos;
}

void Player::spawnPlayer(glm::vec3 pos)
{
	this->m_playerPosition = pos;
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
