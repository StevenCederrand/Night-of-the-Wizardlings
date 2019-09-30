#include <Pch/Pch.h>
#include "PlayState.h"

// TODO move to mesh
#include <Loader/BGLoader.h>
#include <Networking/Client.h>


PlayState::PlayState()
{
	ShaderMap::getInstance()->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");
	ShaderMap::getInstance()->getShader("Basic_Forward")->setInt("albedoTexture", 0);
	Renderer::getInstance();
	m_camera = new Camera();

	//m_player = new Player("test", glm::vec3(0, 2, 3), m_camera);

	m_player = new Player("Player", glm::vec3(0.0f, 1.8f, 0.0f), m_camera);

	Renderer::getInstance()->setupCamera(m_player->getCamera());

	//TODO: organized loading system?
	
	//Test enviroment with 4 meshes inside 1 GameObject, inherited transforms
	m_objects.push_back(new WorldObject("TestScene"));
	m_objects[m_objects.size() - 1]->loadMesh("TestScene.mesh");
	
	//Cube and sphere centered in scene
	m_objects.push_back(new WorldObject("TestCube"));
	m_objects[m_objects.size() - 1]->loadMesh("TestCube.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(0.0f, 0.0f, -2.0f));
	m_objects.push_back(new WorldObject("TestSphere"));
	m_objects[m_objects.size() - 1]->loadMesh("TestSphere.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(5.0f, 1.0f, -2.0f));


	logTrace("Playstate created");

	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();
	ShaderMap::getInstance()->createShader("Skybox_Shader", "Skybox.vs", "Skybox.fs");
	ShaderMap::getInstance()->getShader("Skybox_Shader")->setInt("skyBox", 4);

	m_bPhysics = new BulletPhysics(-10);
	CollisionObject obj = box;
	m_bPhysics->createObject(obj, 0.0f, glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(100.0f, 2.0f, 100.0f), 1.0);
	gContactAddedCallback = callbackFunc;
	m_player->createRigidBody(m_bPhysics);

	for (int i = 1; i < m_objects.size(); i++)
	{
	
		Transform temp = m_objects.at(i)->getTransform();

		m_bPhysics->createObject(obj, 0.0f, temp.position,
			glm::vec3(temp.scale.x/2, temp.scale.y * 10, temp.scale.y/2));
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

	if (col::characterCollided == true)
	{
		m_player->forceUp();
		//m_renderer->getMainCamera()->forceUp();
		col::characterCollided = false;
	}
	
}

void PlayState::render()
{
	Renderer::getInstance()->bindMatrixes(m_player->getCamera()->getViewMat(), m_player->getCamera()->getProjMat());
	Renderer::getInstance()->renderSkybox(*m_skybox);
	m_player->renderSpell();

	auto& list = Client::getInstance()->getNetworkPlayersREF().getPlayersREF();
	/*for (size_t i = 0; i < list.size(); i++)
	{

		if (list[i]->gameobject == nullptr) continue;

		for (int j = 0; j < list[i]->gameobject->getMeshesCount(); j++)
		{
			list[i]->gameobject->bindMaterialToShader("Basic_Forward", j);
			Renderer::getInstance()->render(*list[i]->gameobject, j);
		}

		
	}*/


	

	for (GameObject* object : m_objects)
	{
		for (int i = 0; i < object->getMeshesCount(); i++)
		{
			object->bindMaterialToShader("Basic_Forward", i);
			Renderer::getInstance()->render(*object, i);
		}
	}
}

//This function is called everytime two collision objects collide
bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1,
	const btCollisionObjectWrapper* obj2, int id2, int index2)
{

	if ((Camera*)obj1->getCollisionObject()->getUserPointer() != nullptr)
	{
		//obj1->getCollisionObject;
		if (obj1->getCollisionObject()->getCollisionFlags()
			== (btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK | btCollisionObject::CF_NO_CONTACT_RESPONSE));
		col::characterCollided = true;
	
	}

	return false;
}
