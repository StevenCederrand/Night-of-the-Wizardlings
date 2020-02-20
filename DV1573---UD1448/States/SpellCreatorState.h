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
#include <System/Input.h>
#include <HUD/HudObject.h>
#include <HUD/HudHandler.h>
#include <Spells/AttackSpell.h>

#include <imgui/imgui.h>
//#include <imgui/imfilebrowser.h>

#include "SpellLoader/spellLoader.h"

//bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1,
//	const btCollisionObjectWrapper* obj2, int id2, int index2);

struct Projectiles
{
    int m_ProjectileLowDmg = 0;
    int m_ProjectileHighDmg = 0;
    int m_ProjectileSpeed = 0;
    int m_ProjectileCooldown = 0;
    float m_ProjectileRadius = 0;
    int m_ProjectileLifetime = 0;
    int m_ProjectileMaxBounces = 1;
};

struct AOE
{
    int m_damage = 0;
    int m_AOESpeed = 0;
    int m_AOECooldown = 0;
    int m_AOERadius = 1;
    int m_AOELifetime = 0;
    int m_AOEMaxBounces = 1;
};

struct SpellEvents
{
    int m_nrOfEvents = 1;
    int m_firstEvent = 1;
    int m_secondEvent = 1;
    int m_thirdEvent = 1;
    int m_fourthEvent = 1;
    int m_fifthEvent = 1;
};

class SpellCreatorState : public State {

public:

	SpellCreatorState();
	virtual ~SpellCreatorState() override;
	virtual void update(float dt) override;
	virtual void render() override;

    void updateToolSettings();

    //-----Edit spell Functions-----//
    void editAttackSpell();
    void editAOEAttackSpell();

    //-----Edit spell Events function-----//
    void editSpellEvents();
    void setSpellEvents(int eventNr);

    //IMGUI
    bool my_tool_active = true;
    bool isProjectile = false;
    bool isAOE = false;
    bool isEnhanceProjectile = false;
    SpellLoader myLoader;
    bool loadASpell = false;
    //-----test data-----//
    std::string m_name = "fireball";
    Projectiles normalSpell;
    AOE aoeSpell;
    SpellEvents spellEvents;
    //-----File opener/Path to Exports-----//
   // ImGui::FileBrowser fileDialog;

    char m_spellName[NAME_SIZE];

	bool is_ImGui() { return true; }

private:
    Player* m_player;
    SpellHandler* m_spellHandler;
    BulletPhysics* m_bPhysics;
    std::vector<Pointlight*> m_pointlights;
    SkyBox* m_skybox;
    Camera* m_camera;
    std::vector<GameObject*> m_objects;

    AttackSpell* normalAttSpell;

    bool m_AttackSpellAlive = true;
    bool m_FireSpellAlive = true;

};
#endif



// Notes
// # Added _CRT_SECURE_NO_WARNINGS in: Properties/ C/C++ / Preprocessor
// # Changed the language standard in: Properties/ C/C++ / Language
