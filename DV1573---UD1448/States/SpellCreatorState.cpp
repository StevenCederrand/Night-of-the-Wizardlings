#include <Pch/Pch.h>
#include "SpellCreatorState.h"
#include <System/StateManager.h>
#include "MenuState.h"
#include <Networking/Client.h>
#include <Networking/LocalServer.h>


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
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(glfwGetCurrentContext(), true);
    ImGui::StyleColorsDark();


    MaterialMap::getInstance();

    for (size_t i = 0; i < m_objects.size(); i++)
    {
        m_objects.at(i)->createRigidBody(CollisionObject::box, m_bPhysics);
        //m_objects.at(i)->createDebugDrawer();
    }

    if (Client::getInstance()->isInitialized())
        Client::getInstance()->assignSpellHandler(m_spellHandler);


    fileDialog.SetTitle("Open file(.spell)");
    fileDialog.SetTypeFilters({ ".spell" });
    fileDialog.SetPwd("\Exports");

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

    // SHUTDOWN
    ImGui::DestroyContext();

}

void SpellCreatorState::update(float dt)
{

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

    ImGui_ImplGlfwGL3_NewFrame();
    //ImGui::ShowDemoWindow();

    ImGui::Begin("Spell Creator", &my_tool_active, ImGuiWindowFlags_MenuBar);// Create a window called "Spell Creator" and append into it.
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Select a spell type to create");
    ImGui::Checkbox("Create Projectile", &isProjectile);
    ImGui::Checkbox("Create AOE", &isAOE);

    if (isProjectile)
    {
        isAOE = false;

        editAttackSpell();
    }
    if (isAOE)
    {
        isProjectile = false;
        editAOEAttackSpell();
    }
    fileDialog.Display();

    if (fileDialog.HasSelected() && loadASpell == true)
    {
        // LOAD AN ATTACKSPELL
        std::cout << "Selected filename: " << fileDialog.GetSelected().string() << std::endl;
        myLoader.LoadSpell(m_name);

        //Set the tool values to match the loaded file
        m_ProjectileLowDmg = myLoader.getProjectileLowDmg();
        m_ProjectileHighDmg = myLoader.getProjectileHighDmg();
        m_ProjectileSpeed = myLoader.getProjectileSpeed();
        m_ProjectileCooldown = myLoader.getProjectileCooldown();
        m_ProjectileRadius = myLoader.getProjectileRadius();
        m_ProjectileLifetime = myLoader.getProjectileLifetime();
        m_ProjectileMaxBounces = myLoader.getProjectileMaxBounces();
        fileDialog.ClearSelected();
        loadASpell = false;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Load Spell...", "Ctrl+L"))
            {
                loadASpell = true;
                // open file dialog when user clicks this button
                fileDialog.Open();
            }
            if (ImGui::MenuItem("Save Spell...", "Ctrl+S"))
            {
                loadASpell = false;
                m_name = m_spellName;
                myLoader.SaveSpell(m_name, m_ProjectileLowDmg, m_ProjectileHighDmg, m_ProjectileSpeed, m_ProjectileCooldown, m_ProjectileRadius, m_ProjectileLifetime, m_ProjectileMaxBounces);
            }
            if (ImGui::MenuItem("Close Window", "Ctrl+W"))
            {
                my_tool_active = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::Text("");											            // Display some text (you can use a format strings too)
    if (my_tool_active == false)
    {
        ImGui::CloseCurrentPopup();
    }
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Name your spell:");
    ImGui::InputText("", m_spellName, NAME_SIZE);
    ImGui::End();

}

void SpellCreatorState::render()
{
    Renderer::getInstance()->render();

    ImGui::Render();
    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

}

void SpellCreatorState::editAttackSpell()
{
    if (m_AttackSpellAlive == true)
    {
        m_spellHandler->changeSpell(0);
        m_spellHandler->createSpellForTool(glm::vec3(3, 3, -10), glm::vec3(0, 0, 0), NORMALATTACKTOOL);
        m_AttackSpellAlive = false;
        m_FireSpellAlive = true;
    }

    ImGui::Text("Edit Projectile values");											            // Display some text (you can use a format strings too)
    ImGui::SliderFloat("Spell low Damage", &m_ProjectileLowDmg, 0.0f, 100.0f);
    ImGui::SliderFloat("Spell high Damage", &m_ProjectileHighDmg, 0.0f, 100.0f);
    ImGui::SliderFloat("Spell Speed", &m_ProjectileSpeed, 0.0f, 200.0f);
    ImGui::SliderFloat("Spell Radius", &m_ProjectileRadius, 0.1f, 10.0f);
    ImGui::SliderFloat("Spell Cooldown", &m_ProjectileCooldown, 0.1f, 10.0f);
    ImGui::SliderFloat("Spell Lifetime", &m_ProjectileLifetime, 1.0f, 20.0f);
    ImGui::SliderInt("Spell Maximum Bounces", &m_ProjectileMaxBounces, 1, 3);
}

void SpellCreatorState::editAOEAttackSpell()
{
    if (m_FireSpellAlive == true)
    {
        m_spellHandler->changeSpell(1);
        m_spellHandler->createSpellForTool(glm::vec3(3, 3, -20), glm::vec3(0, 0, 0), FIRETOOL);
        m_FireSpellAlive = false;
        m_AttackSpellAlive = true;
    }

    ImGui::Text("Edit area of effect values");											            // Display some text (you can use a format strings too)
    ImGui::SliderFloat("Spell low Damage", &m_ProjectileLowDmg, 0.0f, 100.0f);
    ImGui::SliderFloat("Spell high Damage", &m_ProjectileHighDmg, 0.0f, 100.0f);
    ImGui::SliderFloat("Spell Speed", &m_ProjectileSpeed, 0.0f, 200.0f);
    ImGui::SliderFloat("Spell Radius", &m_ProjectileRadius, 0.1f, 10.0f);
    ImGui::SliderFloat("Spell Cooldown", &m_ProjectileCooldown, 0.1f, 10.0f);
    ImGui::SliderFloat("Spell Lifetime", &m_ProjectileLifetime, 1.0f, 20.0f);
    ImGui::SliderInt("Spell Maximum Bounces", &m_ProjectileMaxBounces, 1, 3);
}