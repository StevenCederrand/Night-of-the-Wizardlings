#ifndef _PLAYSTATE_H
#define _PLAYSTATE_H
#include <Pch/Pch.h>
#include "System/State.h"
#include <GFX/MaterialMap.h>
#include <GameObject/WorldObject.h>


class PlayState : public State {

public:
	PlayState();
	virtual ~PlayState() override;
	virtual void update(float dt) override;
	virtual void render() override;

private:
	int key = 1;
	Renderer* m_renderer;
	ShaderMap* m_shaderMap;
	Mesh m_mesh;

	WorldObject* m_object;
};



#endif