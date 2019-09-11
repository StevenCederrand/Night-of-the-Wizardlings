#ifndef _LOCAL_SERVER_H
#define _LOCAL_SERVER_H
#include <Pch/Pch.h>

class LocalServer
{
public:

	LocalServer();
	~LocalServer();

	void startup(const std::string& serverName);
	void process();

private:
	unsigned char getPacketID(RakNet::Packet* p);

private:
	RakNet::RakPeerInterface* m_serverPeer;
};

#endif
