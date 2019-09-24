#ifndef _PLAYSTATE_H
#define _PLAYSTATE_H
#include <Pch/Pch.h>
#include "System/State.h"

#include <GFX/MaterialMap.h>
#include <GameObject/GameObject.h>
#include <Player/Player.h>
#include <GameObject/WorldObject.h>


class PlayState : public State {

public:
	PlayState();
	virtual ~PlayState() override;
	virtual void update(float dt) override;
	virtual void render() override;

private:
	int key = 1;

	//Any inherited GameObject class added to this vector will support basic rendering
	std::vector<GameObject*> m_objects;

	Player* m_player;
	Camera* m_camera;

};



#endif