#include <Pch/Pch.h>
#include "PlayState.h"

PlayState::PlayState()
{
	logTrace("Playstate created");
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
