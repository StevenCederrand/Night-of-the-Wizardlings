#ifndef _CHAOS_SERVER_MODE_H
#define _CHAOS_SERVER_MODE_H
#include <Pch/Pch.h>


class ChaosServerMode {

public:
	ChaosServerMode();
	~ChaosServerMode();

	void update(const RakNet::BitStream& bitstream, unsigned char& packetID, RakNet::RakNetGUID sender ,const std::vector<PlayerPacket>& players);
	void registerCallbackOnServerStateChange(std::function<void(NetGlobals::ServerState)> callback);

private:
	std::function<void(NetGlobals::ServerState)> m_registeredOnServerStateChangeCallback;

};

#endif
