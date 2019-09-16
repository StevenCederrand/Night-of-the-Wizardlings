#include <Pch/Pch.h>
#include "PlayState.h"
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>


PlayState::PlayState()
{

	logTrace("Playstate created");
	
	CEGUI::OpenGL3Renderer& guiRenderer = CEGUI::OpenGL3Renderer::bootstrapSystem();
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
}
