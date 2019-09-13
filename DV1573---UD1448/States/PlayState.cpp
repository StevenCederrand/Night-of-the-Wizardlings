#include <Pch/Pch.h>
#include "PlayState.h"

PlayState::PlayState()
{
	BGLoader load;
	load.LoadMesh("C:/Users/Nicolas Wallenskog/Documents/GitHub/DV1573---UD1448/Assets/Meshes/SexyCube.meh");
	Mesh mesh1;
	mesh1.setUpMesh(load.GetVertices(0), load.GetVertexCount(0), load.GetFaces(0), load.GetFaceCount(0));
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
