#include <Pch/Pch.h>
#include "SpellCreatorState.h"
#include <System/StateManager.h>
#include "MenuState.h"
#include <Networking/Client.h>
#include <Networking/LocalServer.h>



#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <stdio.h>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

//#define PLAYSECTION "PLAYSTATE"


//void logVec3(glm::vec3 vector) {
//	logTrace("Vector: ({0}, {1}, {2})", std::to_string(vector.x), std::to_string(vector.y), std::to_string(vector.z));
//}


SpellCreatorState::SpellCreatorState()
{
	std::cout << "SpellCreator State Created! Have fun!" << std::endl;

    m_bPhysics = new BulletPhysics(-20.0f);
    m_spellHandler = new SpellHandler(m_bPhysics);
    //m_spellHandler->setOnHitCallback(std::bind(&PlayState::onSpellHit_callback, this));

    ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setInt("albedoTexture", 0);

    m_camera = new Camera();

    m_player = new Player(m_bPhysics, "Player", NetGlobals::PlayerFirstSpawnPoint, m_camera, m_spellHandler);

    Renderer::getInstance()->setupCamera(m_player->getCamera());

    //TODO: organized loading system?
    m_skybox = new SkyBox();
    m_skybox->prepareBuffers();


    m_player->setHealth(NetGlobals::PlayerMaxHealth);


    m_objects.push_back(new MapObject("internalTestmap"));
    m_objects[m_objects.size() - 1]->loadMesh("map1.mesh");
    m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);

    /*m_objects.push_back(new WorldObject("sphere"));
    m_objects[m_objects.size() - 1]->loadMesh("TestSphere.mesh");
    m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(10.0f, 2.0f, -20.0f));
    Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);*/

    m_objects.push_back(new WorldObject("Character"));
    m_objects[m_objects.size() - 1]->loadMesh("CharacterTest.mesh");
    m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(10.0f, 1.8f, -24.0f));

    Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], STATIC);


    m_objects.push_back(new Deflect("playerShield"));
    m_objects[m_objects.size() - 1]->loadMesh("ShieldMesh.mesh");
    m_objects[m_objects.size() - 1]->setWorldPosition(glm::vec3(10.0f, 13.0f, 6.0f));
    Renderer::getInstance()->submit(m_objects[m_objects.size() - 1], SHIELD);



    MaterialMap::getInstance();
    //gContactAddedCallback = callbackFunc;
    // Geneterate bullet objects / hitboxes
    for (size_t i = 0; i < m_objects.size(); i++)
    {
        m_objects.at(i)->createRigidBody(CollisionObject::box, m_bPhysics);
        //m_objects.at(i)->createDebugDrawer();
    }

    if (Client::getInstance()->isInitialized())
        Client::getInstance()->assignSpellHandler(m_spellHandler);

  //  m_hudHandler.loadPlayStateHUD();
   // m_hideHUD = false;
 
}

SpellCreatorState::~SpellCreatorState()
{
}

void SpellCreatorState::update(float dt)
{
}

void SpellCreatorState::render()
{
}
