#include <Pch/Pch.h>
#include "PlayState.h"
// TODO move to mesh
#include <Loader/BGLoader.h>
#include <Networking/Client.h>

PlayState::PlayState()
{
	ShaderMap::getInstance()->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");
	Renderer::getInstance();
	m_camera = new Camera();
	m_player = new Player("test", glm::vec3(0, 0, 3), m_camera);
	Renderer::getInstance()->setupCamera(m_player->getCamera());


	//TODO: organized loading system?
	m_objects.push_back(new WorldObject("Character"));
	m_objects[m_objects.size() - 1]->loadMesh("WalkingTest.mesh");
	
	
	BGLoader tempLoader;
	tempLoader.LoadMesh(MESHPATH + "WalkingTest.mesh");
	m_objects.push_back(new WorldObject("Level_1"));
	m_objects[m_objects.size() - 1]->loadMesh("TestScene.mesh");


	m_objects.push_back(new WorldObject("OneCubeBoy"));
	m_objects[m_objects.size() - 1]->loadMesh("SexyCube.mesh");


	logTrace("Playstate created");

}

PlayState::~PlayState()
{
	MaterialMap::getInstance()->destroy();

	delete m_player;

	for (GameObject* object : m_objects)
		delete object;
}

void PlayState::update(float dt)
{	
	/*if (Client::getInstance()->isInitialized && Client::getInstance()->isConnectedToSever())
	{
		for (size_t i = 0; i < Client::getInstance()->getConnectedPlayers().size; i++)
		{
			Client::getInstance()->getConnectedPlayers()[i].updateGameObject(dt);
		}
	}*/
	Renderer::getInstance()->update(dt);
	//m_renderer->update(dt);
	m_player->update(dt);
	
}

void PlayState::render()
{
	Renderer::getInstance()->bindMatrixes(m_player->getCamera()->getViewMat(), m_player->getCamera()->getProjMat());

	auto& clientsObject = Client::getInstance()->getConnectedPlayers();
	for (size_t i = 0; i < clientsObject.size(); i++)
	{
		clientsObject[i].getGameObjectPtr()->bindMaterialToShader("Basic_Forward");
		Renderer::getInstance()->render(*clientsObject[i].getGameObjectPtr());
	}

	for (GameObject* object : m_objects)
	{
		object->bindMaterialToShader("Basic_Forward");
		Renderer::getInstance()->render(*object);
	}
}
