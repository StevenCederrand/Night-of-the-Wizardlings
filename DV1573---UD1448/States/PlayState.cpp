#include <Pch/Pch.h>
#include "PlayState.h"
// TODO move to mesh
#include <Loader/BGLoader.h>


PlayState::PlayState()
{
	ShaderMap::getInstance()->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");
	Renderer::getInstance();
	m_camera = new Camera();
	m_player = new Player("Player", glm::vec3(0.0f, 1.8f, 0.0f), m_camera);
	Renderer::getInstance()->setupCamera(m_player->getCamera());

	//TODO: organized loading system?
	
	//Cube and sphere centered in scene
	m_objects.push_back(new WorldObject("TestCube"));
	m_objects[m_objects.size() - 1]->loadMesh("TestCube.mesh");
	m_objects.push_back(new WorldObject("TestSphere"));
	m_objects[m_objects.size() - 1]->loadMesh("TestSphere.mesh");

	//Test enviroment with 4 meshes inside 1 GameObject, inherited transforms
	m_objects.push_back(new WorldObject("TestScene"));
	m_objects[m_objects.size() - 1]->loadMesh("TestScene.mesh");




	ShaderMap::getInstance()->getShader("Basic_Forward")->setInt("albedoTexture", 0);

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
	Renderer::getInstance()->update(dt);
	//m_renderer->update(dt);
	m_player->update(dt);
	
}

void PlayState::render()
{
	Renderer::getInstance()->bindMatrixes(m_player->getCamera()->getViewMat(), m_player->getCamera()->getProjMat());

	for (GameObject* object : m_objects)
	{
		for (int i = 0; i < object->getMeshesCount(); i++)
		{
			object->bindMaterialToShader("Basic_Forward", i);
			Renderer::getInstance()->render(*object, i);
		}
	}
}
