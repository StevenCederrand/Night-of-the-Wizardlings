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

    //m_spellHandler = new SpellHandler();
	m_spellEditor = new SpellEditor();

    Renderer* renderer = Renderer::getInstance();
    renderer->setupCamera(m_camera);

    m_skybox = new SkyBox();
    m_skybox->prepareBuffers();

    renderer->submitSkybox(m_skybox);
    renderer->submitSpellEditor(m_spellEditor);

   

    MaterialMap::getInstance();

    //for (size_t i = 0; i < m_objects.size(); i++)
    //{
    //    m_objects.at(i)->createRigidBody(CollisionObject::box, m_bPhysics);
    //    //m_objects.at(i)->createDebugDrawer();
    //}

  
    fileDialog.SetTitle("Open file(.spell)");
    fileDialog.SetTypeFilters({ ".spell" });
    fileDialog.SetPwd("\Exports");

	textureDialog.SetTitle("Open file(.png)");
	textureDialog.SetTypeFilters({ ".png" });
	textureDialog.SetPwd("\Assets");

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
    delete m_spellEditor;
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

}

void SpellCreatorState::Update(float dt)
{
	update(dt);
	//tempPS.emission = 1 / m_emission;
	m_spellEditor->spellToolUpdate(dt, tempPS, normalSpell, tempTxt, shouldRenderSpell, aoeSpell);
    m_bPhysics->update(dt);

    Renderer::getInstance()->updateParticles(dt);
    auto* clientPtr = Client::getInstance();

    for (GameObject* object : m_objects)
    {
        object->update(dt);
    }
    Renderer::getInstance()->updateParticles(dt);


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
	textureDialog.Display();

    if (fileDialog.HasSelected() && loadASpell == true && isAOE == true)
    {
        // LOAD AN AOESpell
        const std::string path = fileDialog.GetSelected().string();
        auto const pos = path.find_last_of('\\');
        m_name = path.substr(pos + 1);

        myLoader.LoadSpell(m_name, FIRE);

		updateToolSettings(FIRE);

		m_Type = FIRE;
  
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
        myLoader.LoadSpell(m_name, NORMALATTACK);

        updateToolSettings(NORMALATTACK);
    
		m_Type = NORMALATTACK;

        fileDialog.ClearSelected();
        loadASpell = false;
    }

	if (textureDialog.HasSelected())
	{
		const std::string path = textureDialog.GetSelected().string();
		auto const pos = path.find_last_of('\\');
		m_textureName = path.substr(pos + 1);

		tempTxt.name = "Assets/Textures/" + m_textureName;

		myLoader.SaveSpell(m_name, normalSpell, aoeSpell, spellEvents, tempPS, tempTxt, m_Type);

		updateToolSettings(m_Type);

		textureDialog.ClearSelected();
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
				if (isProjectile)
				{
                    myLoader.SaveSpell(m_name, normalSpell, aoeSpell, spellEvents, tempPS, tempTxt, NORMALATTACK);
				}
                if (isAOE)
                    myLoader.SaveSpell(m_name, normalSpell, aoeSpell, spellEvents, tempPS, tempTxt, FIRE);

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
	ImGui::InputText("", m_spellName, IM_ARRAYSIZE(m_spellName));

    ImGui::End();

}

void SpellCreatorState::render()
{
    Renderer::getInstance()->render();
	//ImGui::Render();
  
}

void SpellCreatorState::loadTexture()
{
}

void SpellCreatorState::updateToolSettings(OBJECT_TYPE type)
{
    //Set the tool values to match the loaded file
	if (type == NORMALATTACK)
	{

		normalSpell.lowDamage = myLoader.m_projectile.lowDamage;
		normalSpell.highDamage = myLoader.m_projectile.highDamage;
		normalSpell.speed = myLoader.m_projectile.speed;
		normalSpell.coolDown = myLoader.m_projectile.coolDown;
		normalSpell.radius = myLoader.m_projectile.radius;
		normalSpell.lifeTime = myLoader.m_projectile.lifeTime;
		normalSpell.maxBounces = myLoader.m_projectile.maxBounces;
		normalSpell.color = myLoader.m_projectile.color;
	}

	if (type == FIRE)
	{
		aoeSpell.damage = myLoader.m_AOESpell.damage;
		aoeSpell.speed = myLoader.m_AOESpell.speed;
		aoeSpell.coolDown = myLoader.m_AOESpell.coolDown;
		aoeSpell.radius = myLoader.m_AOESpell.radius;
		aoeSpell.lifeTime = myLoader.m_AOESpell.lifeTime;
		aoeSpell.maxBounces = myLoader.m_AOESpell.maxBounces;
		aoeSpell.color = myLoader.m_AOESpell.color;

	}

    //-----Spell Events-----//
    spellEvents.nrOfEvents  = myLoader.m_spellEvents.nrOfEvents;
    spellEvents.firstEvent  = myLoader.m_spellEvents.firstEvent;
    spellEvents.secondEvent = myLoader.m_spellEvents.secondEvent;
    spellEvents.thirdEvent  = myLoader.m_spellEvents.thirdEvent;
    spellEvents.fourthEvent = myLoader.m_spellEvents.fourthEvent;
    spellEvents.fifthEvent  = myLoader.m_spellEvents.fifthEvent;


	tempTxt.name = myLoader.m_txtInfo.name;
	tempPS = myLoader.m_psInfo;
	tempPS.seed = 0;
	tempPS.direction = glm::vec3(1.0f, 0.0f, 0.0f);
	tempPS.direction = glm::clamp(tempPS.direction, -1.0f, 1.0f);

}

void SpellCreatorState::editAttackSpell()
{
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Edit Projectile values:");								            // Display some text (you can use a format strings too)
    ImGui::SliderFloat("Spell low Damage", &normalSpell.lowDamage, 0.0f, 100.0f);
    ImGui::SliderFloat("Spell high Damage", &normalSpell.highDamage, 0.0f, 100.0f);
    ImGui::SliderFloat("Spell Speed", &normalSpell.speed, 0.0f, 200.0f);
    ImGui::SliderFloat("Spell Radius", &normalSpell.radius, 0.1f, 5.0f);
    ImGui::SliderFloat("Spell Cooldown", &normalSpell.coolDown, 0.1f, 10.0f);
    ImGui::SliderFloat("Spell Lifetime", &normalSpell.lifeTime, 1.0f, 20.0f);
    ImGui::SliderInt("Spell Maximum Bounces", &normalSpell.maxBounces, 0, 5);
	ImGui::ColorEdit3("Color for spell", &normalSpell.color.x);
	ImGui::Checkbox("Render Spell object", &shouldRenderSpell);

    if (m_AttackSpellAlive == true)
    {
		m_spellEditor->changeSpell(0);
		m_spellEditor->createSpellForTool(glm::vec3(3, 3, -10), glm::vec3(0, 0, 0), NORMALATTACKTOOL);
        m_AttackSpellAlive = false;
        m_FireSpellAlive = true;
    }
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Edit particle values:");										            // Display some text (you can use a format strings too)
	ImGui::SliderInt("Number of systems", &nrOfParticleSystems.nrOfEvents, 1.0f, 5.0f);

	if (nrOfParticleSystems.nrOfEvents >= 1.0f)
	{
		
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Particle values 1:");	
		if (ImGui::MenuItem("Load texture to spell...", "Ctrl+L"))
		{
			//loadASpell = true;
			// open file dialog when user clicks this button
			textureDialog.Open();
		}
		ImGui::SliderFloat("Width", &tempPS.width, 0.0f, 10.0f);
		ImGui::SliderFloat("Heigth", &tempPS.heigth, 0.0f, 10.0f);
		ImGui::SliderFloat("Lifetime", &tempPS.lifetime, 0.0f, 100.0f);
		ImGui::SliderInt("Max particles", &tempPS.maxParticles, 0, 5000);
		ImGui::SliderFloat("Emission", &tempPS.emission, 1.0f, 0.00001f);
		ImGui::SliderFloat("Force", &tempPS.force, -100.0f, 100.0f);
		ImGui::SliderFloat("Drag", &tempPS.drag, -100.0f, 100.0f);
		ImGui::SliderFloat("Gravity", &tempPS.gravity, -10.0f, 10.0f);
		//ImGui::SliderInt("Seed", &tempPS.seed, 0.0f, 100.0f);
		ImGui::SliderFloat("Spread", &tempPS.spread, 0.0f, 10.0f);
		ImGui::SliderFloat("Glow", &tempPS.glow, 0.0f, 10.0f);
		ImGui::SliderInt("Scale direction", &tempPS.scaleDirection, 0.0f, 100.0f);
		ImGui::SliderInt("Swirl", &tempPS.swirl, 0.0f, 1.0f);
		ImGui::SliderInt("Fade", &tempPS.fade, -1.0f, 1.0f);
		ImGui::Checkbox("Random spawn", &tempPS.randomSpawn);
		ImGui::Checkbox("Continue", &tempPS.cont);
		ImGui::Checkbox("Omni", &tempPS.omnious);
		ImGui::ColorEdit3("Color2", &tempPS.color.x);
		ImGui::ColorEdit3("Blend color", &tempPS.blendColor.x);
		//ImGui::SliderInt("Direction", &tempPS.direction, 0.0f, 10.0f);
		
		//std::cout << tempPS.color.x << " " << tempPS.color.y << " " << tempPS.color.z << std::endl;

	}
	if (nrOfParticleSystems.nrOfEvents >= 2.0f)
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Particle values 2:");										            // Display some text (you can use a format strings too)
								            // Display some text (you can use a format strings too)

	}
	if (nrOfParticleSystems.nrOfEvents >= 3.0f)
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Edit aret values:");										            // Display some text (you can use a format strings too)

	}
	if (nrOfParticleSystems.nrOfEvents >= 4.0f)
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Edit aret values:");										            // Display some text (you can use a format strings too)

	}
	if (nrOfParticleSystems.nrOfEvents >= 5.0f)
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Edit aret values:");										            // Display some text (you can use a format strings too)

	}
}

void SpellCreatorState::editAOEAttackSpell()
{
    if (m_FireSpellAlive == true)
    {
		m_spellEditor->changeSpell(1);
		m_spellEditor->createSpellForTool(glm::vec3(3, 1, -20), glm::vec3(0, 0, 0), FIRETOOL);
        m_FireSpellAlive = false;
        m_AttackSpellAlive = true;
    }
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Edit area of effect values:");										            // Display some text (you can use a format strings too)
    ImGui::SliderFloat("Spell Damage", &aoeSpell.damage, 0.0f, 100.0f);
    ImGui::SliderFloat("Spell Radius", &aoeSpell.radius, 1.0f, 20.0f);
    ImGui::SliderInt("Spell Lifetime", &aoeSpell.lifeTime, 1.0f, 20.0f);
	ImGui::ColorEdit3("Color for spell ", &aoeSpell.color.x);
	ImGui::Checkbox("Render Spell object", &shouldRenderSpell);

	ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Edit potion values:");										            // Display some text (you can use a format strings too)
    ImGui::SliderFloat("Spell Speed", &aoeSpell.speed, 0.0f, 200.0f);
    ImGui::SliderFloat("Spell Cooldown", &aoeSpell.coolDown, 0.1f, 10.0f);
    ImGui::SliderInt("Spell Maximum Bounces", &aoeSpell.maxBounces, 1, 3);

	ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Edit particle values:");										            // Display some text (you can use a format strings too)
	ImGui::SliderInt("Number of systems", &nrOfParticleSystems.nrOfEvents, 1.0f, 5.0f);

	if (nrOfParticleSystems.nrOfEvents >= 1.0f)
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Particle values 1:");
		if (ImGui::MenuItem("Load texture to spell...", "Ctrl+L"))
		{
			//loadASpell = true;
			// open file dialog when user clicks this button
			textureDialog.Open();
		}
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Particle values 1:");										            // Display some text (you can use a format strings too)
		ImGui::SliderFloat("Width ", &tempPS.width, 0.0f, 10.0f);
		ImGui::SliderFloat("Heigth ", &tempPS.heigth, 0.0f, 10.0f);
		ImGui::SliderFloat("Lifetime ", &tempPS.lifetime, 0.0f, 100.0f);
		ImGui::SliderInt("Max particles ", &tempPS.maxParticles, 0, 5000);
		ImGui::SliderFloat("Emission ", &tempPS.emission, 1.0f, 0.00001f);
		ImGui::SliderFloat("Force ", &tempPS.force, -100.0f, 100.0f);
		ImGui::SliderFloat("Drag ", &tempPS.drag, -100.0f, 100.0f);
		ImGui::SliderFloat("Gravity ", &tempPS.gravity, -100.0f, 100.0f);
		//ImGui::SliderInt("Seed", &tempPS.seed, 0.0f, 100.0f);
		ImGui::SliderFloat("Spread ", &tempPS.spread, 0.0f, 10.0f);
		ImGui::SliderFloat("Glow ", &tempPS.glow, 0.0f, 10.0f);
		ImGui::SliderInt("Scale direction ", &tempPS.scaleDirection, 0.0f, 100.0f);
		ImGui::SliderInt("Swirl ", &tempPS.swirl, 0.0f, 1.0f);
		ImGui::SliderInt("Fade ", &tempPS.fade, -1.0f, 1.0f);
		ImGui::Checkbox("Random spawn ", &tempPS.randomSpawn);
		ImGui::Checkbox("Continue ", &tempPS.cont);
		ImGui::Checkbox("Omni ", &tempPS.omnious);
		ImGui::ColorEdit3("Color ", &tempPS.color.x);
		ImGui::ColorEdit3("Blend color ", &tempPS.blendColor.x);
		//ImGui::SliderInt("Direction", &tempPS.direction, 0.0f, 10.0f);
		
		//std::cout << tempPS.color.x << " " << tempPS.color.y << " " << tempPS.color.z << std::endl;

	}
	if (nrOfParticleSystems.nrOfEvents >= 2.0f)
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Particle values 2:");										            // Display some text (you can use a format strings too)
								            // Display some text (you can use a format strings too)

	}
	if (nrOfParticleSystems.nrOfEvents >= 3.0f)
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Edit aret values:");										            // Display some text (you can use a format strings too)

	}
	if (nrOfParticleSystems.nrOfEvents >= 4.0f)
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Edit aret values:");										            // Display some text (you can use a format strings too)

	}
	if (nrOfParticleSystems.nrOfEvents >= 5.0f)
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Edit aret values:");										            // Display some text (you can use a format strings too)

	}

}

void SpellCreatorState::editSpellEvents()
{
    ImGui::SliderInt(" ", &spellEvents.nrOfEvents, 1.0f, 5.0f);
    ImGui::Text("");
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Select Event(s):");
    if (spellEvents.nrOfEvents >= 1)
    {
        setSpellEvents(spellEvents.firstEvent);
        ImGui::SliderInt("Start Event", &spellEvents.firstEvent, 1, 3);
    }
    if (spellEvents.nrOfEvents >= 2)
    {

        setSpellEvents(spellEvents.secondEvent);
        ImGui::SliderInt("  ", &spellEvents.secondEvent, 1, 5);
    }
    if (spellEvents.nrOfEvents >= 3)
    {
        setSpellEvents(spellEvents.thirdEvent);
        ImGui::SliderInt("   ", &spellEvents.thirdEvent, 1, 5);

    }
    if (spellEvents.nrOfEvents >= 4)
    {
        setSpellEvents(spellEvents.fourthEvent);
        ImGui::SliderInt("    ", &spellEvents.fourthEvent, 1, 5);

    }
    if (spellEvents.nrOfEvents >= 5)
    {
        setSpellEvents(spellEvents.fifthEvent);
        ImGui::SliderInt("     ", &spellEvents.fifthEvent, 1, 5);
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
        ImGui::Text("Spawn Tripleattack-Spell");
    }
    if (eventNr == 4.0f)
    {
        ImGui::Text("Spawn AOE (Area only)");
    }
    if (eventNr == 5.0f)
    {
        ImGui::Text("Bounce");
    }
    if (eventNr == 6.0f)
    {
        ImGui::Text("Change Attribute of the next Projectile-Spell");
    }
    if (eventNr == 7.0f)
    {
        ImGui::Text("event7");
    }
    if (eventNr == 8.0f)
    {
        ImGui::Text("event8");
    }
    if (eventNr == 9.0f)
    {
        ImGui::Text("event9");
    }
    if (eventNr == 10.0f)
    {
        ImGui::Text("event10");
    }
}

void SpellCreatorState::guiInfo()
{
	Update(DeltaTime);
}
