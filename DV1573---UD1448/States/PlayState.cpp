#include <Pch/Pch.h>
#include "PlayState.h"

PlayState::PlayState()
{
	logTrace("Playstate created");
	ShaderMap::createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");


	BGLoader tempLoader;
	tempLoader.LoadMesh("Assets/Meshes/SexyCubex2.meh");

	Mesh mesh1;
	mesh1.setUpMesh(tempLoader.GetVertices(0),
		tempLoader.GetVertexCount(0),
		tempLoader.GetFaces(0),
		tempLoader.GetFaceCount(0));
	m_shaderMap = m_shaderMap->getInstance();
	m_shaderMap->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");
	m_renderer = m_renderer->getInstance();

	m_cube = new Cube(m_renderer->m_VBO);
}

PlayState::~PlayState()
{
	delete m_cube;
	logTrace("Playstate destroyed");
}

void PlayState::update(float dt)
{
	m_renderer->update(dt);
}

void PlayState::render()
{
	m_renderer->render(m_cube);

}
