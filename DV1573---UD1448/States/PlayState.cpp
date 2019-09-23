#include <Pch/Pch.h>
#include "PlayState.h"
// TODO move to mesh
#include <Loader/BGLoader.h>


PlayState::PlayState()
{
	ShaderMap::getInstance()->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");
	Renderer::getInstance();
	m_camera = new Camera();
	m_player = new Player("test", glm::vec3(0, 0, 3), m_camera);
	Renderer::getInstance()->setupCamera(m_camera);


	//TODO: organized loading system?
	m_objects.push_back(new WorldObject("Character"));
	m_objects[m_objects.size() - 1]->loadMesh("WalkingTest.mesh");



	
	/*
	BGLoader tempLoader;
	tempLoader.LoadMesh(MESHPATH + "WalkingTest.mesh");
	m_objects.push_back(new WorldObject("Level_1"));
	m_objects[m_objects.size() - 1]->loadMesh("TestScene.mesh");


	m_objects.push_back(new WorldObject("OneCubeBoy"));
	m_objects[m_objects.size() - 1]->loadMesh("SexyCube.mesh");


	logTrace("Playstate created");*/

}

PlayState::~PlayState()
{
	MaterialMap::getInstance()->destroy();

	delete m_camera;

	for (GameObject* object : m_objects)
		delete object;
}

void PlayState::update(float dt)
{
	m_renderer->update(dt);
	m_player->update(dt);
	Renderer::getInstance()->update(dt);
}

void PlayState::render()
{
	Renderer::getInstance()->bindMatrixes(m_camera->getViewMat(), m_camera->getProjMat());

	for (GameObject* object : m_objects)
	{
		object->bindMaterialToShader("Basic_Forward");
		Renderer::getInstance()->render(*object);
	}
}
