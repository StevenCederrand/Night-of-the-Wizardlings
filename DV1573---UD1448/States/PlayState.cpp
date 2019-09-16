#include <Pch/Pch.h>
#include "PlayState.h"

PlayState::PlayState()
{
	logTrace("Playstate created");
	ShaderMap::getInstance()->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");
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
	Renderer::getInstance()->render(m_cube->getBuffers(), m_cube->getModelMatrix());
}
