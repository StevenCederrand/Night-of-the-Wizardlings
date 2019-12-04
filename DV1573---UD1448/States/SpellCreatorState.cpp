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

    m_spellHandler->spellToolUpdate(dt, normalSpell.m_ProjectileRadius, normalSpell.m_ProjectileSpeed);
    m_bPhysics->update(dt);
   // m_player->update(dt);
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

    if (fileDialog.HasSelected() && loadASpell == true && isAOE == true)
    {
        // LOAD AN AOESpell
        const std::string path = fileDialog.GetSelected().string();
        auto const pos = path.find_last_of('\\');
        m_name = path.substr(pos + 1);

        myLoader.loadAOESpell(m_name);

        //Set the tool values to match the loaded file
        aoeSpell.m_damage = myLoader.m_AOESpell.m_damage;
        aoeSpell.m_AOESpeed = myLoader.m_AOESpell.m_speed;
        aoeSpell.m_AOECooldown = myLoader.m_AOESpell.m_coolDown;
        aoeSpell.m_AOERadius = myLoader.m_AOESpell.m_radius;
        aoeSpell.m_AOELifetime = myLoader.m_AOESpell.m_lifeTime;
        aoeSpell.m_AOEMaxBounces = myLoader.m_AOESpell.m_maxBounces;
  
        fileDialog.ClearSelected();
        loadASpell = false;
    }

    if (fileDialog.HasSelected() && loadASpell == true && isProjectile == true)
    {
        // LOAD AN ATTACKSPELL
        const std::string path = fileDialog.GetSelected().string();
        auto const pos = path.find_last_of('\\');
        m_name = path.substr(pos + 1);

        //myLoader.LoadSpell(m_name);
        myLoader.LoadProjectileSpell(m_name);

        updateToolSettings();
    
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
                if(isProjectile)
                    myLoader.SaveProjectileSpell(m_name, normalSpell.m_ProjectileLowDmg, normalSpell.m_ProjectileHighDmg, normalSpell.m_ProjectileSpeed,
                    normalSpell.m_ProjectileCooldown, normalSpell.m_ProjectileRadius, normalSpell.m_ProjectileLifetime, normalSpell.m_ProjectileMaxBounces,
                    spellEvents.m_nrOfEvents, spellEvents.m_firstEvent, spellEvents.m_secondEvent, spellEvents.m_thirdEvent, spellEvents.m_fourthEvent, spellEvents.m_fifthEvent);
                if (isAOE)
                    myLoader.saveAOESpell(m_name, aoeSpell.m_damage, aoeSpell.m_AOESpeed, aoeSpell.m_AOECooldown, aoeSpell.m_AOERadius, aoeSpell.m_AOELifetime, aoeSpell.m_AOEMaxBounces);

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

    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Number of spell events:");
    editSpellEvents();

    ImGui::Text("");
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

void SpellCreatorState::updateToolSettings()
{
    //Set the tool values to match the loaded file
    normalSpell.m_ProjectileLowDmg      = myLoader.m_projectile.m_lowDamage;
    normalSpell.m_ProjectileHighDmg     = myLoader.m_projectile.m_highDamage;
    normalSpell.m_ProjectileSpeed       = myLoader.m_projectile.m_speed;
    normalSpell.m_ProjectileCooldown    = myLoader.m_projectile.m_coolDown;
    normalSpell.m_ProjectileRadius      = myLoader.m_projectile.m_radius;
    normalSpell.m_ProjectileLifetime    = myLoader.m_projectile.m_lifeTime;
    normalSpell.m_ProjectileMaxBounces  = myLoader.m_projectile.m_maxBounces;

    //-----Spell Events-----//
    spellEvents.m_nrOfEvents  = myLoader.m_spellEvents.m_nrOfEvents;
    spellEvents.m_firstEvent  = myLoader.m_spellEvents.m_firstEvent;
    spellEvents.m_secondEvent = myLoader.m_spellEvents.m_secondEvent;
    spellEvents.m_thirdEvent  = myLoader.m_spellEvents.m_thirdEvent;
    spellEvents.m_fourthEvent = myLoader.m_spellEvents.m_fourthEvent;
    spellEvents.m_fifthEvent  = myLoader.m_spellEvents.m_fifthEvent;
}

void SpellCreatorState::editAttackSpell()
{
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Edit Projectile values:");								            // Display some text (you can use a format strings too)
    ImGui::SliderInt("Spell low Damage", &normalSpell.m_ProjectileLowDmg, 0.0f, 100.0f);
    ImGui::SliderInt("Spell high Damage", &normalSpell.m_ProjectileHighDmg, 0.0f, 100.0f);
    ImGui::SliderInt("Spell Speed", &normalSpell.m_ProjectileSpeed, 0.0f, 200.0f);
    ImGui::SliderInt("Spell Radius", &normalSpell.m_ProjectileRadius, 0.1f, 10.0f);
    ImGui::SliderInt("Spell Cooldown", &normalSpell.m_ProjectileCooldown, 0.1f, 10.0f);
    ImGui::SliderInt("Spell Lifetime", &normalSpell.m_ProjectileLifetime, 1.0f, 20.0f);
    ImGui::SliderInt("Spell Maximum Bounces", &normalSpell.m_ProjectileMaxBounces, 0, 5);
    if (m_AttackSpellAlive == true)
    {
        m_spellHandler->changeSpell(0);
        m_spellHandler->createSpellForTool(glm::vec3(3, 3, -10), glm::vec3(0, 0, 0), NORMALATTACKTOOL);
        m_AttackSpellAlive = false;
        m_FireSpellAlive = true;
    }
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
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Edit area of effect values:");										            // Display some text (you can use a format strings too)
    ImGui::SliderInt("Spell Damage", &aoeSpell.m_damage, 0.0f, 100.0f);
    ImGui::SliderInt("Spell Speed", &aoeSpell.m_AOESpeed, 0.0f, 200.0f);
    ImGui::SliderInt("Spell Radius", &aoeSpell.m_AOERadius, 1.0f, 50.0f);
    ImGui::SliderInt("Spell Cooldown", &aoeSpell.m_AOECooldown, 0.1f, 10.0f);
    ImGui::SliderInt("Spell Lifetime", &aoeSpell.m_AOELifetime, 1.0f, 20.0f);
    ImGui::SliderInt("Spell Maximum Bounces", &aoeSpell.m_AOEMaxBounces, 1, 3);
}

void SpellCreatorState::editSpellEvents()
{
    ImGui::SliderInt(" ", &spellEvents.m_nrOfEvents, 1.0f, 5.0f);
    ImGui::Text("");
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Select Event(s):");
    if (spellEvents.m_nrOfEvents >= 1.0f)
    {
        setSpellEvents(spellEvents.m_firstEvent);
        ImGui::SliderInt("Event one", &spellEvents.m_firstEvent, 1.0f, 2.0f);
    }
    if (spellEvents.m_nrOfEvents >= 2.0f)
    {
        setSpellEvents(spellEvents.m_secondEvent);
        ImGui::SliderInt("Event two", &spellEvents.m_secondEvent, 1.0f, 5.0f);
    }
    if (spellEvents.m_nrOfEvents >= 3.0f)
    {
        setSpellEvents(spellEvents.m_thirdEvent);
        ImGui::SliderInt("Event three", &spellEvents.m_thirdEvent, 1.0f, 5.0f);

    }
    if (spellEvents.m_nrOfEvents >= 4.0f)
    {
        setSpellEvents(spellEvents.m_fourthEvent);
        ImGui::SliderInt("Event four", &spellEvents.m_fourthEvent, 1.0f, 5.0f);

    }
    if (spellEvents.m_nrOfEvents >= 5.0f)
    {
        setSpellEvents(spellEvents.m_fifthEvent);
        ImGui::SliderInt("Event five", &spellEvents.m_fifthEvent, 1.0f, 5.0f);
    }
}

void SpellCreatorState::setSpellEvents(int eventNr)
{
    if (eventNr == 1.0f)
    {
        ImGui::Text("Spawn Projectile-Spell");
    }
    if (eventNr == 2.0f)
    {
        ImGui::Text("Spawn AOE-Spell");
    }
    if (eventNr == 3.0f)
    {
        ImGui::Text("Spawn AOE (Area only)");
    }
    if (eventNr == 4.0f)
    {
        ImGui::Text("Bounce");
    }
    if (eventNr == 5.0f)
    {
        ImGui::Text("Change Attribute of the next Projectile-Spell");
    }
}
