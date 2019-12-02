#ifndef _SPELL_CREATOR_STATE_H
#define _SPELL_CREATOR_STATE_H
#include <Pch/Pch.h>
#include <System/State.h>
#include <GUI/Gui.h>
#include <GFX/MaterialMap.h>
#include <GameObject/GameObject.h>
#include <Player/Player.h>
#include <GameObject/WorldObject.h>
#include <GameObject/MapObject.h>
#include <GameObject/AnimatedObject.h>
#include <Spells/Spell.h>
#include <System/BulletPhysics.h>
#include <HUD/HudObject.h>
#include <HUD/HudHandler.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <stdio.h>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <imfilebrowser.h>

#include "spellLoader.h"



//bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1,
//	const btCollisionObjectWrapper* obj2, int id2, int index2);

class SpellCreatorState : public State {

public:


	SpellCreatorState();
	virtual ~SpellCreatorState() override;
	virtual void update(float dt) override;
	virtual void render() override;

    //IMGUI
    bool my_tool_active = true;
    bool isProjectile = false;
    bool isAOE = false;
    bool isEnhanceProjectile = false;
    SpellLoader myLoader;
    //-----test data-----//
    std::string m_name = "fireball";
    float m_ProjectileDmg = 0;
    float m_ProjectileSpeed = 0;
    float m_ProjectileCooldown = 0;
    float m_ProjectileRadius = 0;
    float m_ProjectileLifetime = 0;
    int m_ProjectileMaxBounces = 1;
    //-----File opener/Path to Exports-----//
    ImGui::FileBrowser fileDialog;


private:
    Player* m_player;
    SpellHandler* m_spellHandler;
    BulletPhysics* m_bPhysics;
    //DeflectRender* m_deflectBox;
    SkyBox* m_skybox;
    Camera* m_camera;
    std::vector<GameObject*> m_objects;



};
#endif



// Notes
// # Added _CRT_SECURE_NO_WARNINGS in: Properties/ C/C++ / Preprocessor
// # Changed the language standard in: Properties/ C/C++ / Language
