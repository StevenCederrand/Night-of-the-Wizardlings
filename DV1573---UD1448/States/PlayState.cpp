#include <Pch/Pch.h>
#include "PlayState.h"


// TODO move to mesh
#include <Networking/Client.h>


PlayState::PlayState()
{
	m_bPhysics = new BulletPhysics(-10);
	ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setInt("albedoTexture", 0);
	Renderer::getInstance();
	m_camera = new Camera();
	m_player = new Player(m_bPhysics, "Player", glm::vec3(0.0f, 1.8f, 0.0f), m_camera);

	Renderer::getInstance()->setupCamera(m_player->getCamera());

	//TODO: organized loading system?
	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();

	//Test enviroment with 4 meshes inside 1 GameObject, inherited transforms
	m_objects.push_back(new WorldObject("TestScene"));
	m_objects[m_objects.size() - 1]->loadMesh("TestScene.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);
	
	//Cube and sphere centered in scene
	m_objects.push_back(new WorldObject("TestCube"));
	m_objects[m_objects.size() - 1]->loadMesh("TestCube.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(5.0f, 0.0f, 0.0f));
	Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);

	m_objects.push_back(new WorldObject("TestSphere"));
	m_objects[m_objects.size() - 1]->loadMesh("TestSphere.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(10.0f, 2.0f, -4.0f));
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(5.0f, 1.0f, -2.0f));
	Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);

	m_objects.push_back(new WorldObject("TestCube"));
	m_objects[m_objects.size() - 1]->loadMesh("TestCube.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(10.0f, 2.0f, -1.0f));
	Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);
	
	////Animated rectangle
	m_objects.push_back(new AnimatedObject("TestRectangle"));
	m_objects[m_objects.size() - 1]->loadMesh("TestRectangle.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(0.0f, 0.0f, -4.0f));
	Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], ANIMATEDSTATIC);

	//Animated goblino
	m_objects.push_back(new AnimatedObject("TestGoblino"));
	m_objects[m_objects.size() - 1]->loadMesh("ElGoblino.mesh");
	Transform tempTransform;
	tempTransform.scale = glm::vec3(0.03f, 0.03f, 0.03f);
	tempTransform.position = glm::vec3(-3.0f, 0.0f, 3.0f);
	m_objects[m_objects.size() - 1]->setTransform(tempTransform);
	Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], ANIMATEDSTATIC);



	CollisionObject obj = box;
	m_bPhysics->createObject(obj, 0.0f, glm::vec3(0.0f, -1.5f, 0.0f), glm::vec3(100.0f, 2.0f, 100.0f), 1.0);
	gContactAddedCallback = callbackFunc;

	for (int i = 1; i < m_objects.size(); i++)
	{
		Transform temp = m_objects.at(i)->getTransform();

		m_bPhysics->createObject(obj, 0.0f, temp.position,
			glm::vec3(temp.scale.x/2, temp.scale.y, temp.scale.y/2));
	}




	logTrace("Playstate created");
}

PlayState::~PlayState()
{
	logTrace("Deleting playstate..");
	for (GameObject* object : m_objects)
		delete object;
	delete m_skybox;
	delete m_player;
	delete m_bPhysics;

	MaterialMap::getInstance()->destroy();
	MeshMap::getInstance()->destroy();
	AnimationMap::getInstance()->destroy();
	SkeletonMap::getInstance()->destroy();
}

void PlayState::update(float dt)
{	

	Client::getInstance()->updateNetworkEntities(dt);
	m_bPhysics->update(dt);
	Renderer::getInstance()->update(dt);
	m_player->update(dt);
	for (GameObject* object : m_objects)
	{
		object->update(dt);
	}

	if (Input::isKeyPressed(GLFW_KEY_P)) {
		auto& list = Client::getInstance()->getNetworkSpells();
		logTrace("Active spells on client: {0}", list.size());
	}

}

void PlayState::render()
{
	//Move the render skybox to be a private renderer function
	Renderer::getInstance()->renderSkybox(*m_skybox);
	Renderer::getInstance()->render();
	m_player->renderSpell();
//<<<<<<< HEAD
//
//	// Renderer networked players
//	auto& networkedPlayerList = Client::getInstance()->getNetworkPlayersREF().getPlayersREF();
//
//	for (size_t i = 0; i < networkedPlayerList.size(); i++)
//	{
//		if (networkedPlayerList[i]->gameobject == nullptr) continue;
//
//		for (int j = 0; j < networkedPlayerList[i]->gameobject->getMeshesCount(); j++)
//		{
//			networkedPlayerList[i]->gameobject->bindMaterialToShader("Basic_Forward", j);
//			Renderer::getInstance()->render(*networkedPlayerList[i]->gameobject, j);
//
//		}	
//	}
//
//	// Renderer networked Spells
//	auto& networkedSpellList = Client::getInstance()->getNetworkSpellsREF().getSpellEntitiesREF();
//
//	for (size_t i = 0; i < networkedSpellList.size(); i++)
//	{
//		if (networkedSpellList[i].tempObject == nullptr) continue;
//
//		for (int j = 0; j < networkedSpellList[i].tempObject->getMeshesCount(); j++)
//		{
//			networkedSpellList[i].tempObject->bindMaterialToShader("Basic_Forward", j);
//			Renderer::getInstance()->render(*networkedSpellList[i].tempObject, j);
//
//		}
//	}
//
//
//	for (GameObject* object : m_objects)
//	{
//		for (int i = 0; i < object->getMeshesCount(); i++)
//		{
//			object->bindMaterialToShader("Basic_Forward", i);
//			Renderer::getInstance()->render(*object, i);
//		}
//	}

}

//This function is called everytime two collision objects collide
bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1,
	const btCollisionObjectWrapper* obj2, int id2, int index2)
{
	return false;
}
