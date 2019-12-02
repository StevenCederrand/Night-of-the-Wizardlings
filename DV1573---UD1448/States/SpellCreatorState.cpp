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

    //-----Set up IMGUI-----//
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(glfwGetCurrentContext(), true);
    ImGui::StyleColorsDark();

    fileDialog.SetTitle("Open file(.spell)");
    fileDialog.SetTypeFilters({ ".spell" });
    fileDialog.SetPwd("\Exports");


}

SpellCreatorState::~SpellCreatorState()
{
}

void SpellCreatorState::update(float dt)
{

    ImGui_ImplGlfwGL3_NewFrame();
    ImGui::ShowDemoWindow();

    ImGui::Begin("Spell Creator", &my_tool_active, ImGuiWindowFlags_MenuBar);// Create a window called "Spell Creator" and append into it.
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Select a spell type to create");
    ImGui::Checkbox("Create Projectile", &isProjectile);
    ImGui::Checkbox("Create AOE", &isAOE);

    if (isProjectile)
    {
        isAOE = false;

        ImGui::Text("Edit Projectile values");											            // Display some text (you can use a format strings too)
        ImGui::SliderFloat("Spell Damage", &m_ProjectileDmg, 10.0f, 40.0f);
        ImGui::SliderFloat("Spell Speed", &m_ProjectileSpeed, 50.0f, 200.0f);
        ImGui::SliderFloat("Spell Cooldown", &m_ProjectileCooldown, 0.1f, 10.0f);
        ImGui::SliderFloat("Spell Radius", &m_ProjectileRadius, 0.1f, 10.0f);
        ImGui::SliderFloat("Spell Lifetime", &m_ProjectileLifetime, 1.0f, 20.0f);
        ImGui::SliderInt("Spell Maximum Bounces", &m_ProjectileMaxBounces, 1, 3);
    }
    if (isAOE)
    {
        isProjectile = false;
        ImGui::Text("Here Area of Effect spell data will be edited...");											            // Display some text (you can use a format strings too)
    }
    fileDialog.Display();

    if (fileDialog.HasSelected())
    {
        std::cout << "Selected filename: " << fileDialog.GetSelected().string() << std::endl;
        myLoader.LoadSpell(m_name);

        //Set the tool values to match the loaded file
        m_ProjectileDmg = myLoader.getProjectileDmg();
        m_ProjectileSpeed = myLoader.getProjectileSpeed();
        m_ProjectileCooldown = myLoader.getProjectileCooldown();
        m_ProjectileRadius = myLoader.getProjectileRadius();
        m_ProjectileLifetime = myLoader.getProjectileLifetime();
        m_ProjectileMaxBounces = myLoader.getProjectileMaxBounces();
        fileDialog.ClearSelected();
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Load Spell...", "Ctrl+L"))
            {
                // open file dialog when user clicks this button
                fileDialog.Open();
            }
            if (ImGui::MenuItem("Save Spell...", "Ctrl+S"))
            {
                myLoader.SaveSpell(m_name, m_ProjectileDmg, m_ProjectileSpeed, m_ProjectileCooldown, m_ProjectileRadius, m_ProjectileLifetime, m_ProjectileMaxBounces);
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
    ImGui::Text("");											            // Display some text (you can use a format strings too)
    if (my_tool_active == false)
    {
        ImGui::CloseCurrentPopup();
    }

    ImGui::End();
}

void SpellCreatorState::render()
{
    ImGui::Render();
    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
  //glfwSwapBuffers(glfwGetCurrentContext());
}
