#ifndef _PLAYSTATE_H
#define _PLAYSTATE_H
#include <Pch/Pch.h>
#include "System/State.h"

class PlayState : public State {

public:
	PlayState();
	virtual ~PlayState() override;
	virtual void update(float dt) override;
	virtual void render() override;

private:

};



#endif