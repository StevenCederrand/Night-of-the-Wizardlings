#include <Pch/Pch.h>
#include "PlayState.h"
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>
// TODO move to mesh
#include <Loader/BGLoader.h>

PlayState::PlayState()
{
	ShaderMap::getInstance()->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");
	Renderer::getInstance();
	m_camera = new Camera();
	Renderer::getInstance()->setupCamera(m_camera);

	// TODO move to mesh and file filepath
	m_object = new WorldObject("Character");
	m_object->loadMesh("TestScene.mesh");
	
	logTrace("Playstate created");

	CEGUI::OpenGL3Renderer& guiRenderer = CEGUI::OpenGL3Renderer::bootstrapSystem();

	/*
	Material tempMaterial;
	tempMaterial = tempLoader.GetMaterial(0);

	
	m_materialMap = m_materialMap->getInstance();
	m_materialMap->createMaterial((std::string)tempLoader.GetMaterial(0).name, tempMaterial);


	tempLoader.Unload();

	m_shaderMap = m_shaderMap->getInstance();
	m_shaderMap->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");

	m_renderer = m_renderer->getInstance();
	m_cube = new Cube();
	m_cube->loadTexture("testTexture.jpg");
	*/
}

PlayState::~PlayState()
{
	MaterialMap::getInstance()->destroy();
	delete m_object;
	delete m_camera;
}

void PlayState::update(float dt)
{
	Renderer::getInstance()->update(dt);
}

void PlayState::render()
{
	//m_object->bindMaterialToShader("Basic_Forward");
	Renderer::getInstance()->bindMatrixes(m_camera->getViewMat(), m_camera->getProjMat());
//	Renderer::getInstance()->render(*m_object);
}
