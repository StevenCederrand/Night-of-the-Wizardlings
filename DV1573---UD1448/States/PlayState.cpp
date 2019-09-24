#include <Pch/Pch.h>
#include "PlayState.h"
// TODO move to mesh
#include <Loader/BGLoader.h>


PlayState::PlayState()
{
	ShaderMap::getInstance()->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");
	Renderer::getInstance();
	m_camera = new Camera();
	Renderer::getInstance()->setupCamera(m_camera);

	//TODO: organized loading system?
	//NOTE: Temporary hardcoded positions
	m_objects.push_back(new WorldObject("TestCube"));
	m_objects[m_objects.size() - 1]->loadMesh("TestCube.mesh");
	m_objects[0]->setWorldPosition(glm::vec3(2.0f, 3.0f, -5.0f));

	m_objects.push_back(new WorldObject("TestSphere"));
	m_objects[m_objects.size() - 1]->loadMesh("TestSphere.mesh");
	m_objects[1]->setWorldPosition(glm::vec3(-2.0f, 3.0f, -5.0f));

	ShaderMap::getInstance()->getShader("Basic_Forward")->setInt("albedoTexture", 0);

	logTrace("Playstate created");

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
	Renderer::getInstance()->update(dt);
}

void PlayState::render()
{
	for (GameObject* object : m_objects)
	{
		object->bindMaterialToShader("Basic_Forward");
		Renderer::getInstance()->bindMatrixes(m_camera->getViewMat(), m_camera->getProjMat());
		Renderer::getInstance()->render(*object);
	}
}
