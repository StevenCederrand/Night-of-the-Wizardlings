#include <Pch/Pch.h>
#include "ChaosServerMode.h"

ChaosServerMode::ChaosServerMode()
{
}

ChaosServerMode::~ChaosServerMode()
{
}

void ChaosServerMode::update(const RakNet::BitStream& bitstream, unsigned char& packetID)
{

}

void ChaosServerMode::registerCallbackOnServerStateChange(std::function<void(NetGlobals::ServerState)> callback)
{
	m_registeredOnServerStateChangeCallback = callback;
}

void ChaosServerMode::testCallback()
{
	NetGlobals::ServerState state = NetGlobals::ServerState::WaitingForPlayers;
	m_registeredOnServerStateChangeCallback(state);
}
