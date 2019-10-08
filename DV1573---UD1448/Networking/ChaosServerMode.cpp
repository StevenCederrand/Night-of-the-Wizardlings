#include <Pch/Pch.h>
#include "ChaosServerMode.h"

ChaosServerMode::ChaosServerMode()
{
}

ChaosServerMode::~ChaosServerMode()
{
}

void ChaosServerMode::update(const RakNet::BitStream& bitstream, unsigned char& packetID, RakNet::RakNetGUID sender, const std::vector<PlayerPacket>& players)
{
	if (packetID == SERVER_CHANGE_STATE) {
		NetGlobals::ServerState state = NetGlobals::ServerState::GameStarted;
		m_registeredOnServerStateChangeCallback(state);
	}

}

void ChaosServerMode::registerCallbackOnServerStateChange(std::function<void(NetGlobals::ServerState)> callback)
{
	m_registeredOnServerStateChangeCallback = callback;
}

