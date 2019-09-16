#include <Pch/Pch.h>
#include "PlayState.h"

PlayState::PlayState()
{
	logTrace("Playstate created");


	BGLoader tempLoader;
	tempLoader.LoadMesh("Assets/Meshes/SexyCube3.meh");

	m_mesh.setUpMesh(tempLoader.GetVertices(0),
		tempLoader.GetVertexCount(0),
		tempLoader.GetFaces(0),
		tempLoader.GetFaceCount(0));
	m_mesh.setUpBuffers();
	tempLoader.Unload();


	m_shaderMap = m_shaderMap->getInstance();
	m_shaderMap->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");

	m_renderer = m_renderer->getInstance();
	m_cube = new Cube();
	m_cube->loadTexture("testTexture.jpg");
}

PlayState::~PlayState()
{
	delete m_cube;
}

void PlayState::update(float dt)
{
	m_renderer->update(dt);
}

void PlayState::render()
{
	Renderer::getInstance()->render(m_mesh.getBuffers(), m_mesh.getPos());
}
