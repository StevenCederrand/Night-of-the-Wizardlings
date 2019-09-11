#ifndef _CLIENT_H
#define _CLIENT_H
#include <Pch/Pch.h>

class Client
{
public:
	Client();
	~Client();

	void startup();
	void connectToAnotherServer(const ServerInfo& server);
	void connectToMyServer();
	void process();
	const std::vector<ServerInfo>& getServerList() const;



	void refreshServerList();
	bool doneRefreshingServerList();
	

private:
	void findAllServerAddresses();
	
	unsigned char getPacketID(RakNet::Packet* p);

private:
	RakNet::RakPeerInterface* m_clientPeer;
	std::vector<ServerInfo> m_serverList;
	bool m_isRefreshingServerList;
	bool m_isConnectedToAnServer;
};

#endif