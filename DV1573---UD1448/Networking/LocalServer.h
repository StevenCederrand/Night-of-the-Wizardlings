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
	bool handleLostPlayer(const RakNet::Packet& packet, const RakNet::BitStream& bsIn);
	void stateChange(NetGlobals::SERVER_STATE newState);
	void sendStreamToAllClients(RakNet::BitStream& stream);

	// Collision with players
	bool specificSpellCollision(const glm::vec3& spellPos, const glm::vec3& playerPos, const std::vector<glm::vec3>& axis);
	glm::vec3 OBBclosestPoint(const glm::vec3& spherePos, const std::vector<glm::vec3>& axis, const glm::vec3& playerPos);
	PlayerPacket* getSpecificPlayer(const RakNet::RakNetGUID& guid);
	SpellPacket* getSpecificSpell(const uint64_t& creatorGUID, const uint64_t& spellID);

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
