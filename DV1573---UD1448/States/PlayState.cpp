#include <Pch/Pch.h>
#include "PlayState.h"

// TODO move to mesh
#include <Networking/Client.h>


PlayState::PlayState()
{
	ShaderMap::getInstance()->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");
	ShaderMap::getInstance()->getShader("Basic_Forward")->setInt("albedoTexture", 0);
	ShaderMap::getInstance()->createShader("Animation", "Animation.vs", "FragShader.fs");
	Renderer::getInstance();
	m_camera = new Camera();
	m_player = new Player("Player", glm::vec3(0.0f, 1.8f, 0.0f), m_camera);
	Renderer::getInstance()->setupCamera(m_player->getCamera());

	//TODO: organized loading system?
	
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


	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();
	ShaderMap::getInstance()->createShader("Skybox_Shader", "Skybox.vs", "Skybox.fs");
	ShaderMap::getInstance()->getShader("Skybox_Shader")->setInt("skyBox", 4);
	
	logTrace("Playstate created");
}

PlayState::~PlayState()
{
	logTrace("Deleting playstate..");

	MaterialMap::getInstance()->destroy();
	MeshMap::getInstance()->destroy();
	AnimationMap::getInstance()->destroy();
	SkeletonMap::getInstance()->destroy();
	
	delete m_skybox;
	delete m_player;
	
	for (GameObject* object : m_objects)
		delete object;
}

void PlayState::update(float dt)
{	
	Client::getInstance()->updateNetworkedPlayers(dt);
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
