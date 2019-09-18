#ifndef _LOBBY_STATE_H
#define _LOBBY_STATE_H
#include <Pch/Pch.h>
#include "System/State.h"

class LobbyState : public State {

public:
	LobbyState();
	virtual ~LobbyState() override;
	virtual void update(float dt) override;
	virtual void render() override;

private:

};


#endif
