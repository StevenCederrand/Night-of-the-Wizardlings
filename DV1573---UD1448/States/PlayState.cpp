#include <Pch/Pch.h>
#include "PlayState.h"


// TODO move to mesh
#include <Loader/BGLoader.h>
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
	
	//Test enviroment with 4 meshes inside 1 GameObject, inherited transforms
	m_objects.push_back(new WorldObject("TestScene"));
	m_objects[m_objects.size() - 1]->loadMesh("TestScene.mesh");
	Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);
	
	//Cube and sphere centered in scene
	m_objects.push_back(new WorldObject("TestCube"));
	m_objects[m_objects.size() - 1]->loadMesh("TestCube.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(0.0f, 0.0f, -2.0f));
	Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);
	m_objects.push_back(new WorldObject("TestSphere"));
	m_objects[m_objects.size() - 1]->loadMesh("TestSphere.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(5.0f, 1.0f, -2.0f));
	Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);


	logTrace("Playstate created");

	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();
	ShaderMap::getInstance()->createShader("Skybox_Shader", "Skybox.vs", "Skybox.fs");
	ShaderMap::getInstance()->getShader("Skybox_Shader")->setInt("skyBox", 4);

	CollisionObject obj = box;
	m_bPhysics->createObject(obj, 0.0f, glm::vec3(0.0f, -1.5f, 0.0f), glm::vec3(100.0f, 2.0f, 100.0f), 1.0);
	gContactAddedCallback = callbackFunc;
	

	for (int i = 1; i < m_objects.size(); i++)
	{
		Transform temp = m_objects.at(i)->getTransform();

		m_bPhysics->createObject(obj, 0.0f, temp.position,
			glm::vec3(temp.scale.x/2, temp.scale.y, temp.scale.y/2));
	}
}

PlayState::~PlayState()
{
	logTrace("Deleting playstate..");

	MaterialMap::getInstance()->destroy();
	MeshMap::getInstance()->destroy();
	
	delete m_skybox;
	delete m_player;
	delete m_bPhysics;
	for (GameObject* object : m_objects)
		delete object;

}

void PlayState::update(float dt)
{	

	Client::getInstance()->updateNetworkedPlayers(dt);
	m_bPhysics->update(dt);
	Renderer::getInstance()->update(dt);
	m_player->update(dt);

}

void PlayState::render()
{
	//Move the render skybox to be a private renderer function
	Renderer::getInstance()->renderSkybox(*m_skybox);
	
	m_player->renderSpell();
	
	Renderer::getInstance()->render();
}

//This function is called everytime two collision objects collide
bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1,
	const btCollisionObjectWrapper* obj2, int id2, int index2)
{
	return false;
}
