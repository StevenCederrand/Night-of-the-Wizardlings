#ifndef _LEVEL_EDIT_STATE_H
#define _LEVEL_EDIT_STATE_H
#include <Pch/Pch.h>
#include <System/State.h>
#include <GUI/Gui.h>
#include <GFX/MaterialMap.h> //<-- ?
#include <GameObject/GameObject.h>
#include <Player/Player.h>
#include <GameObject/WorldObject.h>
#include <GameObject/MapObject.h>
#include <GameObject/AnimatedObject.h>
#include <GameObject/DestructibleObject.h>
#include <Spells/Spell.h> //<-- Dont think this will be needed
#include <System/BulletPhysics.h>
#include <HUD/HudObject.h> 
#include <HUD/HudHandler.h>
#include <GFX/Pointlight.h>
#include <System/MemoryUsage.h>
#include <HUD/WorldHudObject.h>
#include <LevelEditor/MousePicker.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui/imfilebrowser.h>




class LevelEditState : public State
{
public:
	LevelEditState(bool cameraState);
	~LevelEditState();

	void loadMap(); 
	void loadCanvas();
	void loadDecor();
	void saveMap();
	void loadBasicLight();
	void deleteMesh();

	virtual void update(float dt) override;
	virtual void render() override;

private:
	void updateState(const float& dt);

private:
	//Any inherited GameObject class added to this vector will support basic rendering
	std::vector<GameObject*> m_objects;
	std::vector<GameObject*> m_models;
	std::vector<Pointlight*> m_pointlights;

	Camera* m_camera;
	SkyBox* m_skybox;
	MousePicker* m_picker;

	float m_rotVal;
	glm::vec3 m_focusPoint;
	ImGui::FileBrowser fileDialog;

	bool my_tool_active = true;
};



#endif