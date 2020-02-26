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
#include <imgui/ImGuizmo.h>

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

// Solution for handling std::vector<std::string> in list boxes
// Based on https://eliasdaler.github.io/using-imgui-with-sfml-pt2/#combobox-listbox solution
static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_text = vector.at(idx).c_str();
	return true;
};

class LevelEditState : public State
{
public:
	LevelEditState();
	~LevelEditState();
	
	std::string OpenFileDialog(const char* filter, HWND owner);
	void saveLevel();
	void loadBasicLight();
	void cleanScene();

	/*bool GetVecToStr(void* data, int i, const char** out_text);*/

	virtual void update(float dt) override;
	virtual void render() override;
	bool is_ImGui() { return true; }
	void guiInfo();

	/*static bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1,
		const btCollisionObjectWrapper* obj2, int id2, int index2);*/

private:
	void loadAsset(std::vector<GameObject*>&, std::string);
	void addInstance(std::vector<GameObject*>&, std::string);
	void updateState(const float& dt);

	bool vecOfStrGet(void* data, int n, const char** out_text);

private:
	//Any inherited GameObject class added to this vector will support basic rendering
	std::vector<GameObject*> m_objects;
	std::vector<Pointlight*> m_pointlights;

	std::vector <std::string> m_objectNames;
	std::vector<const char*> m_LightsNames;
	std::vector<const char*> m_ParticlesNames;

	int m_nrOfObj = 0;
	int m_nrOfLight = 0;
	int m_nrOfPartic = 0;

	Camera* m_camera;
	SkyBox* m_skybox;
	MousePicker* m_picker;

	float m_rotVal;
	bool tool_active;
	int changeAttrib = 1;
	glm::vec3 m_focusPoint;

	std::string objectName;
	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values);

	//Loaded Meshes

	//ImGui Variables
	int assetTab = 0;

	static const char * items[];
	int selectedItem = 0;

};



#endif