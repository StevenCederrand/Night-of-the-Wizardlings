#include <Pch/Pch.h>
#include "PlayState.h"

PlayState::PlayState()
{
	logTrace("Playstate created");
	ShaderMap::createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");


	BGLoader tempLoader;
	tempLoader.LoadMesh("C:/Users/fisk0/Documents/GitHub/DV1573---UD1448/Assets/Meshes/SexyCubex2.meh");
	tempLoader.GetVertices(0);
	tempLoader.GetFaces(0);

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
