#include <Pch/Pch.h>
#include "PlayState.h"

PlayState::PlayState()
{
	logTrace("Playstate created");
	m_shaderMap = m_shaderMap->getInstance();
	m_shaderMap->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");
	m_renderer = m_renderer->getInstance();
}

PlayState::~PlayState()
{
	logTrace("Playstate destroyed");
}

void PlayState::update(float dt)
{
	
}

void PlayState::render()
{
	m_shaderMap->useByName("Basic_Forward");
	m_renderer->drawQuad();
	
}
