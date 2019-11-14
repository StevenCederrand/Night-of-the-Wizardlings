#ifndef _PLAYSTATE_H
#define _PLAYSTATE_H
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

bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1,
	const btCollisionObjectWrapper* obj2, int id2, int index2);

class PlayState : public State {

public:
	PlayState();
	virtual ~PlayState() override;
	virtual void update(float dt) override;
	virtual void render() override;

private:
	/* Callbacks */
	void onSpellHit_callback();

private:
	int key = 1;
	float m_rotVal;
	bool m_GUIOpen;
	
	//Any inherited GameObject class added to this vector will support basic rendering
	std::vector<GameObject*> m_objects;
	AnimatedObject* m_firstPerson;
	SpellHandler* m_spellHandler;
	Player* m_player;
	Camera* m_camera;
	SkyBox* m_skybox;
	DeflectRender* m_deflectBox;
	BulletPhysics* m_bPhysics;

	HudHandler m_hudHandler;
	
	CEGUI::PushButton* m_mainMenu;
	CEGUI::PushButton* m_quit;
	CEGUI::MultiColumnList* m_scoreBoard;
	bool m_scoreboardExists;
	bool m_endGameBoardVisible;
	bool m_hideHUD;

	glm::vec3 m_lastPositionOfMyKiller;

private: 

	bool onMainMenuClick(const CEGUI::EventArgs& e);
	bool onQuitClick(const CEGUI::EventArgs& e);
	
	void HUDHandler();
	void GUIHandler();
	void GUILoadScoreboard();
	void GUILoadButtons();
	void GUIclear();
};



#endif