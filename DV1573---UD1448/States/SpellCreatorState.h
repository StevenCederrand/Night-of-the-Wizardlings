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

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <stdio.h>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>


//bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1,
//	const btCollisionObjectWrapper* obj2, int id2, int index2);

class SpellCreatorState : public State {

public:


	SpellCreatorState();
	virtual ~SpellCreatorState() override;
	virtual void update(float dt) override;
	virtual void render() override;
    void chooseSpell();


private:
    Player* m_player;
    SpellHandler* m_spellHandler;
    BulletPhysics* m_bPhysics;
    //DeflectRender* m_deflectBox;
    std::vector<Pointlight*> m_pointlights;
    SkyBox* m_skybox;
    Camera* m_camera;
    std::vector<GameObject*> m_objects;



};
#endif
