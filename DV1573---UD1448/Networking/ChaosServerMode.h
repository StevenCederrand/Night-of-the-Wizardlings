#ifndef _CHAOS_SERVER_MODE_H
#define _CHAOS_SERVER_MODE_H
#include <Pch/Pch.h>


class ChaosServerMode {

public:
	ChaosServerMode();
	~ChaosServerMode();

	void update(const RakNet::BitStream& bitstream, unsigned char& packetID);
	void registerCallbackOnServerStateChange(std::function<void(NetGlobals::ServerState)> callback);

	void testCallback();
private:
	std::function<void(NetGlobals::ServerState)> m_registeredOnServerStateChangeCallback;

};


#endif
