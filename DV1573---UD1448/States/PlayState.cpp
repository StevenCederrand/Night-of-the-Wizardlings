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
