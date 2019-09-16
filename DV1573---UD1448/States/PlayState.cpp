#include <Pch/Pch.h>
#include "PlayState.h"



PlayState::PlayState()
{

	logTrace("Playstate created");
	
	CEGUI::OpenGL3Renderer& guiRenderer = CEGUI::OpenGL3Renderer::bootstrapSystem();
}

PlayState::~PlayState()
{
	logTrace("Playstate destroyed");
	CEGUI::System::destroy();
}

void PlayState::update(float dt)
{
	
}

void PlayState::render()
{
}
