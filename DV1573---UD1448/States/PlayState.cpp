#include <Pch/Pch.h>
#include "PlayState.h"
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>
// TODO move to mesh
#include <Loader/BGLoader.h>

PlayState::PlayState()
{
	m_shaderMap = m_shaderMap->getInstance();
	m_shaderMap->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");


	// TODO move to mesh and file filepath
	m_object = new WorldObject("Character");
	m_object->loadMesh("TestScene.mesh");
	

	m_renderer = m_renderer->getInstance();
	logTrace("Playstate created");

	CEGUI::OpenGL3Renderer& guiRenderer = CEGUI::OpenGL3Renderer::bootstrapSystem();
}

PlayState::~PlayState()
{
	MaterialMap::getInstance()->destroy();
}

void PlayState::update(float dt)
{
	m_renderer->update(dt);
}

void PlayState::render()
{
	
	m_object->bindMaterialToShader("Basic_Forward");
	Renderer::getInstance()->render(*m_object);
}
