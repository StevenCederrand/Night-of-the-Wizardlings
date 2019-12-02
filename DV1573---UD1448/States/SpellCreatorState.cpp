#include <Pch/Pch.h>
#include "SpellCreatorState.h"
#include <System/StateManager.h>
#include "MenuState.h"
#include <Networking/Client.h>
#include <Networking/LocalServer.h>



//#include <imgui.h>
//#include <imgui_impl_glfw_gl3.h>
//#include <stdio.h>
//#include <GL/glew.h> 
//#include <GLFW/glfw3.h>

//#define PLAYSECTION "PLAYSTATE"


//void logVec3(glm::vec3 vector) {
//	logTrace("Vector: ({0}, {1}, {2})", std::to_string(vector.x), std::to_string(vector.y), std::to_string(vector.z));
//}


SpellCreatorState::SpellCreatorState()
{
	std::cout << "SpellCreator State Created! Have fun!" << std::endl;

    ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setInt("albedoTexture", 0);

	m_camera = new Camera();
	m_bPhysics = new BulletPhysics(-20.0f);

    GameObject* AnimationMesh = new WorldObject("AnimationMesh");
    AnimationMesh->loadMesh("NyCharacter.mesh");
    delete AnimationMesh;

    m_spellHandler = new SpellHandler(m_bPhysics);

    Renderer* renderer = Renderer::getInstance();
    renderer->setupCamera(m_camera);

    m_skybox = new SkyBox();
    m_skybox->prepareBuffers();

    renderer->submitSkybox(m_skybox);
    renderer->submitSpellhandler(m_spellHandler);

    //-----Set up IMGUI-----//
  /*  ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(glfwGetCurrentContext(), true);
    ImGui::StyleColorsDark();*/


    MaterialMap::getInstance();

    for (size_t i = 0; i < m_objects.size(); i++)
    {
        m_objects.at(i)->createRigidBody(CollisionObject::box, m_bPhysics);
        //m_objects.at(i)->createDebugDrawer();
    }

    if (Client::getInstance()->isInitialized())
        Client::getInstance()->assignSpellHandler(m_spellHandler);
}

SpellCreatorState::~SpellCreatorState()
{

    for (GameObject* object : m_objects)
        delete object;

    m_pointlights.clear();
    m_objects.clear();

    delete m_skybox;
    delete m_player;
    delete m_bPhysics;
    delete m_spellHandler;
    delete m_camera;

    if (LocalServer::getInstance()->isInitialized()) {
        LocalServer::getInstance()->destroy();
    }

    if (Client::getInstance()->isInitialized()) {
        Client::getInstance()->destroy();
    }

    MeshMap::getInstance()->cleanUp();


 
}

void SpellCreatorState::update(float dt)
{

    //ImGui_ImplGlfwGL3_NewFrame();
    //ImGui::ShowDemoWindow();
    chooseSpell();

    m_bPhysics->update(dt);
   // m_player->update(dt);
    m_spellHandler->spellToolUpdate(dt);
    Renderer::getInstance()->updateParticles(dt);
    auto* clientPtr = Client::getInstance();

    for (GameObject* object : m_objects)
    {
        object->update(dt);
    }
    Renderer::getInstance()->updateParticles(dt);
}

void SpellCreatorState::render()
{
    Renderer::getInstance()->render();

    //ImGui::Render();

   // ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
  //  glfwSwapBuffers(glfwGetCurrentContext());
}

void SpellCreatorState::chooseSpell()
{
    if (Input::isKeyPressed(GLFW_KEY_Q))
    {
        m_spellHandler->createSpellForTool(glm::vec3(0, 3, -10), glm::vec3(0, 0, 0), NORMALATTACKTOOL);
    }
    if (Input::isKeyPressed(GLFW_KEY_E))
    {
        m_spellHandler->createSpellForTool(glm::vec3(0, 3, -20), glm::vec3(0, 0, 0), FIRETOOL);
    }
}
