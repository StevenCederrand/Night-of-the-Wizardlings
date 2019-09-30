#include <Pch/Pch.h>
#include "PlayState.h"

// TODO move to mesh
#include <Networking/Client.h>


PlayState::PlayState()
{
	ShaderMap::getInstance()->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");
	ShaderMap::getInstance()->getShader("Basic_Forward")->setInt("albedoTexture", 0);
	Renderer::getInstance();
	m_camera = new Camera();
	m_player = new Player("Player", glm::vec3(0.0f, 1.8f, 0.0f), m_camera);
	Renderer::getInstance()->setupCamera(m_player->getCamera());

	//TODO: organized loading system?
	
	//Test enviroment with 4 meshes inside 1 GameObject, inherited transforms
	m_objects.push_back(new WorldObject("TestScene"));
	m_objects[m_objects.size() - 1]->loadMesh("TestScene.mesh");
	
	//Cube and sphere centered in scene
	m_objects.push_back(new WorldObject("TestCube"));
	m_objects[m_objects.size() - 1]->loadMesh("TestCube.mesh");
	m_objects.push_back(new WorldObject("TestSphere"));
	m_objects[m_objects.size() - 1]->loadMesh("TestSphere.mesh");

	m_objects.push_back(new AnimatedObject("AnimationTest"));
	m_objects[m_objects.size() - 1]->loadMesh("WalkingTest.mesh");
	Transform tempTransform;
	tempTransform.scale = glm::vec3(0.3f, 0.3f, 0.3f);
	m_objects[m_objects.size() - 1]->setTransform(tempTransform);
	m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(-3.0f, 0.0f, 3.0f));


	logTrace("Playstate created");

	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();
	ShaderMap::getInstance()->createShader("Skybox_Shader", "Skybox.vs", "Skybox.fs");
	ShaderMap::getInstance()->getShader("Skybox_Shader")->setInt("skyBox", 4);
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


	

	for (GameObject* object : m_objects)
	{
		for (int i = 0; i < object->getMeshesCount(); i++)
		{
			object->bindMaterialToShader("Basic_Forward", i);
			Renderer::getInstance()->render(*object, i);
		}
	}
}
