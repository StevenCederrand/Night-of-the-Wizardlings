#include <Pch/Pch.h>
#include "PlayState.h"

PlayState::PlayState()
{


	BGLoader tempLoader;
	tempLoader.LoadMesh("Assets/Meshes/SexyCube3.meh");

	m_mesh.setUpMesh(tempLoader.GetVertices(0),
		tempLoader.GetVertexCount(0),
		tempLoader.GetFaces(0),
		tempLoader.GetFaceCount(0));
	m_mesh.setUpBuffers();
	tempLoader.Unload();


	m_shaderMap = m_shaderMap->getInstance();
	m_shaderMap->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");

	m_renderer = m_renderer->getInstance();
	m_cube = new Cube();
	m_cube->loadTexture("testTexture.jpg");
	
	m_gui.init();
	m_gui.loadScheme("TaharezLook.scheme");
	m_gui.setFont("DejaVuSans-10");
	CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.5f, 0.5f, 0.1f, 0.05f), glm::vec4(0.0f), "TestButton"));
	btn->setText("Hello");

	CEGUI::Combobox* combobox = static_cast<CEGUI::Combobox*>(m_gui.createWidget("TaharezLook/Combobox", glm::vec4(0.5f, 0.6f, 0.1f, 0.05f), glm::vec4(0.0f), "TestCombobox"));

	CEGUI::Combobox* combobox2 = static_cast<CEGUI::Combobox*>(m_gui.createWidget("TaharezLook/Combobox", glm::vec4(0.5f, 0.7f, 0.1f, 0.05f), glm::vec4(0.0f), "TestCombobox2"));

	m_gui.setMouseCursor("TaharezLook/MouseArrow");
	m_gui.showMouseCursor();

}

PlayState::~PlayState()
{
	m_gui.destroy();
	delete m_cube;
}

void PlayState::update(float dt)
{

	m_gui.update(dt);

	m_renderer->update(dt);
}

void PlayState::render()
{
	//Renderer::getInstance()->render(m_mesh.getBuffers(), m_mesh.getPos());

	m_gui.draw();
}
