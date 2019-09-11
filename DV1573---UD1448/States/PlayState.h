#ifndef _PLAYSTATE_H
#define _PLAYSTATE_H
#include <Pch/Pch.h>
#include "System/State.h"
#include <Networking/LocalServer.h>
#include <Networking/Client.h>


class PlayState : public State {

public:
	PlayState();
	virtual ~PlayState() override;
	virtual void update(float dt) override;
	virtual void render() override;

private:

	LocalServer m_server;
	Client m_client;
	bool m_isServer;
	bool m_isClient;

};



#endif