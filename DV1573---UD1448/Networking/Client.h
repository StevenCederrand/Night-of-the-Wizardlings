#ifndef _CLIENT_H
#define _CLIENT_H
#include <Pch/Pch.h>
#include "NetworkPlayer.h"

class Client
{
public:
	Client();
	~Client();

	void startup();
	void connectToAnotherServer(const ServerInfo& server);
	void connectToMyServer();
	void threadedProcess();
	const std::vector<ServerInfo>& getServerList() const;
	const std::vector<NetworkPlayer>& getConnectedPlayers() const;
	void refreshServerList();
	bool doneRefreshingServerList();
	

private:
	void findAllServerAddresses();
	unsigned char getPacketID(RakNet::Packet* p);

	void printAllConnectedPlayers();

private:
	RakNet::RakPeerInterface* m_clientPeer;
	RakNet::SystemAddress m_serverAddress;
	std::vector<ServerInfo> m_serverList;
	bool m_isRefreshingServerList;
	bool m_isConnectedToAnServer;
	
	std::thread m_processThread;
	bool m_shutdownClient;

	std::vector<NetworkPlayer> m_connectedPlayers;

};

#endif