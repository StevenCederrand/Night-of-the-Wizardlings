#include <Pch/Pch.h>
#include "LevelEditState.h"
#include <System/StateManager.h>
#include "MenuState.h"

LevelEditState::LevelEditState(bool cameraState)
{
	ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setInt("albedoTexture", 0);

	m_camera = new Camera(glm::vec3(57.f, 100.f, -78.f), -232.0f, -43.2);

	m_picker = new MousePicker(m_camera, m_camera->getProjMat());

	if (cameraState == true)
	{
		m_camera->setSpectatorMode(SpectatorMode::FreeCamera);
	}

	Renderer* renderer = Renderer::getInstance();
	renderer->setupCamera(m_camera);

	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();
	renderer->submitSkybox(m_skybox);
	

	//the load map function might need tweaking in this regard
	//We also need a save map func.
	//loadMap();
}

LevelEditState::~LevelEditState()
{
	for (GameObject* object : m_objects)
		delete object;
	for (Pointlight* light : m_pointlights)
		delete light;

	//Delete UI

	m_pointlights.clear();
	m_objects.clear();

	delete m_camera;
	delete m_skybox;
	
	MeshMap::getInstance()->cleanUp();
}

void LevelEditState::loadMap()
{
	Renderer* renderer = Renderer::getInstance();
	m_objects.push_back(new MapObject("AcademyMap"));
	m_objects[m_objects.size() - 1]->loadMesh("Towermap/Academy_t.mesh");
	renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::STATIC);
}

void LevelEditState::loadDecor()
{
	Renderer* renderer = Renderer::getInstance();
	m_objects.push_back(new MapObject("Academy_Outer"));
	m_objects[m_objects.size() - 1]->loadMesh("ExteriorTest.mesh");
	renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::STATIC);
}

void LevelEditState::saveMap()
{
}

void LevelEditState::loadBasicLight()
{
	Renderer* renderer = Renderer::getInstance();
	// Church
	m_pointlights.emplace_back(new Pointlight(glm::vec3(49.0f, 15.0f, 2.0f), glm::vec3(0.3, 0.85, 1.0)));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 65.0f));

	// Middle
	m_pointlights.emplace_back(new Pointlight(glm::vec3(0.0f, 24.0f, 0.0f), glm::vec3(0.9, 0.17, 0.123)));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.14f, 0.07f, 47.0f));

	// Court area
	m_pointlights.emplace_back(new Pointlight(glm::vec3(-41.0f, 21.0f, 10.0f), glm::vec3(0.9, 0.2, 0.5)));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.045f, 0.0075f, 100.0f));

	// Back wall platforms M
	m_pointlights.emplace_back(new Pointlight(glm::vec3(-2.0f, 19.0f, -31.0f), glm::vec3(0.98, 0.675, 0.084)));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.14f, 0.11f, 47.0f));

	// Back wall platforms R
	m_pointlights.emplace_back(new Pointlight(glm::vec3(-31.0f, 17.0f, -37.0f), glm::vec3(0.98, 0.675, 0.084)));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.14, 0.11f, 47.0f));

	// Back wall platforms L
	m_pointlights.emplace_back(new Pointlight(glm::vec3(29.0f, 19.0f, -37.0f), glm::vec3(0.98, 0.675, 0.084)));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.14f, 0.11f, 47.0f));

	// Maze
	m_pointlights.emplace_back(new Pointlight(glm::vec3(-100.0f, 13.0f, -4.0f), glm::vec3(0.9, 0.9, 1.0)));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.09f, 0.032f, 64.0f));

	for (Pointlight* p : m_pointlights)
		renderer->submit(p, RENDER_TYPE::POINTLIGHT_SOURCE);
}

void LevelEditState::update(float dt)
{
	updateState(dt);

	static float t = 0.0f;
	t += DeltaTime;
	
	//Check for input
	if (Input::isKeyPressed(GLFW_KEY_M))
		loadMap();
	if (Input::isKeyPressed(GLFW_KEY_L))
		loadBasicLight();
	if (Input::isKeyPressed(GLFW_KEY_K))
		loadDecor();
}

void LevelEditState::render()
{
	Renderer::getInstance()->render();
}


void LevelEditState::updateState(const float& dt)
{
	for (GameObject* object : m_objects)
		if (object != nullptr)
			object->update(dt);

	m_camera->updateLevelEd();
	m_picker->update();
	logTrace("MousePicker: ({0}, {1}, {2})", std::to_string(m_picker->getCurrentRay().x), std::to_string(m_picker->getCurrentRay().y), std::to_string(m_picker->getCurrentRay().z));
	Renderer::getInstance()->updateParticles(dt);
	for (GameObject* object : m_objects)
		object->update(dt);

}
