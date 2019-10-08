#include <Pch/Pch.h>
#include "PlayState.h"


// TODO move to mesh
#include <Networking/Client.h>


PlayState::PlayState()
{
	m_bPhysics = new BulletPhysics(-10);
	ShaderMap::getInstance()->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");
	ShaderMap::getInstance()->getShader("Basic_Forward")->setInt("albedoTexture", 0);
	ShaderMap::getInstance()->createShader("Animation", "Animation.vs", "FragShader.fs");
	Renderer::getInstance();
	m_camera = new Camera();
	m_player = new Player(m_bPhysics, "Player", glm::vec3(0.0f, 1.8f, 0.0f), m_camera);

	Renderer::getInstance()->setupCamera(m_player->getCamera());

	//TODO: organized loading system?
	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();
	ShaderMap::getInstance()->createShader("Skybox_Shader", "Skybox.vs", "Skybox.fs");
	ShaderMap::getInstance()->getShader("Skybox_Shader")->setInt("skyBox", 4);

	//Test enviroment with 4 meshes inside 1 GameObject, inherited transforms
	m_objects.push_back(new WorldObject("TestScene"));
	m_objects[m_objects.size() - 1]->loadMesh("TestScene.mesh");

	//Animated rectangle
	m_objects.push_back(new AnimatedObject("TestRectangle"));
	m_objects[m_objects.size() - 1]->loadMesh("TestRectangle.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(0.0f, 0.0f, -4.0f));

	m_objects.push_back(new WorldObject("TestSphere"));
	m_objects[m_objects.size() - 1]->loadMesh("TestSphere.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(10.0f, 2.0f, -4.0f));
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(5.0f, 1.0f, -2.0f));

	m_objects.push_back(new WorldObject("TestCube"));
	m_objects[m_objects.size() - 1]->loadMesh("TestCube.mesh");
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(10.0f, 2.0f, -1.0f));

	//Animated goblino
	m_objects.push_back(new AnimatedObject("TestGoblino"));
	m_objects[m_objects.size() - 1]->loadMesh("ElGoblino.mesh");
	Transform tempTransform;
	tempTransform.scale = glm::vec3(0.03f, 0.03f, 0.03f);
	m_objects[m_objects.size() - 1]->setTransform(tempTransform);
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(-3.0f, 0.0f, 3.0f));




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

	Client::getInstance()->updateNetworkedPlayers(dt);
	m_bPhysics->update(dt);
	Renderer::getInstance()->update(dt);
	m_player->update(dt);
	for (GameObject* object : m_objects)
	{
		object->update(dt);
	}

}

void PlayState::render()
{
	Renderer::getInstance()->bindMatrixes(m_player->getCamera()->getViewMat(), m_player->getCamera()->getProjMat());
	Renderer::getInstance()->renderSkybox(*m_skybox);
	m_player->renderSpell();

	// Network render
	auto& list = Client::getInstance()->getNetworkPlayersREF().getPlayersREF();

	for (size_t i = 0; i < list.size(); i++)
	{
		if (list[i]->gameobject == nullptr) continue;

		for (int j = 0; j < list[i]->gameobject->getMeshesCount(); j++)
		{
			list[i]->gameobject->bindMaterialToShader("Basic_Forward", j);
			Renderer::getInstance()->render(*list[i]->gameobject, j);

		}	
	}

	// Default
	for (GameObject* object : m_objects)
	{
		for (int i = 0; i < object->getMeshesCount(); i++)
		{
			if (object->getType() == 0)
			{
				object->bindMaterialToShader("Basic_Forward", i);
				Renderer::getInstance()->render(*object, i);
			}
			else if (object->getType() == 1)
			{
				object->bindMaterialToShader("Animation", i);
				static_cast<AnimatedObject*>(object)->BindMatrix(i);
				Renderer::getInstance()->renderAni(*object, i);
			}
		}
	}
}

//This function is called everytime two collision objects collide
bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1,
	const btCollisionObjectWrapper* obj2, int id2, int index2)
{
	return false;
}
