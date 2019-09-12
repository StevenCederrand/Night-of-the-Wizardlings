#include <Pch/Pch.h>
#include "PlayState.h"

PlayState::PlayState()
{
	logTrace("Playstate created");
	ShaderMap::createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");
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
	ShaderMap::useByName("Basic_Forward");
	Renderer::render();
}
