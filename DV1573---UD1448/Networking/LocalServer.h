#ifndef _LOCAL_SERVER_H
#define _LOCAL_SERVER_H
#include <Pch/Pch.h>

class LocalServer
{
public:
	LocalServer();
	~LocalServer();

	static LocalServer* getInstance();

	void startup(const std::string& serverName);
	void destroy();
	void ThreadedUpdate();
	void processAndHandlePackets();
	const bool& isInitialized() const;
	const ServerInfo& getMySeverInfo() const;

private:
	unsigned char getPacketID(RakNet::Packet* p);
	void handleLostPlayer(const RakNet::Packet& packet, const RakNet::BitStream& bsIn);
	void stateChange(NetGlobals::ServerState newState);
	void sendStreamToAllClients(RakNet::BitStream& stream);
private:
	RakNet::RakPeerInterface* m_serverPeer = nullptr;
	std::thread m_processThread;
	bool m_shutdownServer;
	std::mutex m_cleanupMutex;
	std::vector<PlayerPacket> m_connectedPlayers;
	std::unordered_map<uint64_t, std::vector<SpellPacket>> m_activeSpells;

	ServerInfo m_serverInfo;
	bool m_initialized = false;
	RakNet::RakNetGUID m_adminID;

};

#endif
