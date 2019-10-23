#ifndef _LOCAL_SERVER_H
#define _LOCAL_SERVER_H
#include <Pch/Pch.h>
#include <System/TimedCallback.h>

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
	struct Respawner {
		uint32_t currentTime = NetGlobals::timeUntilRespawnMS;
		PlayerPacket* player;
	};

private:
	unsigned char getPacketID(RakNet::Packet* p);
	bool handleLostPlayer(const RakNet::Packet& packet, const RakNet::BitStream& bsIn);
	void stateChange(NetGlobals::SERVER_STATE newState);
	void sendStreamToAllClients(RakNet::BitStream& stream);

	// Collision with players
	bool specificSpellCollision(const SpellPacket& spellPacket, const glm::vec3& playerPos, const std::vector<glm::vec3>& axis);
	glm::vec3 OBBclosestPoint(const SpellPacket& spellPacket, const std::vector<glm::vec3>& axis, const glm::vec3& playerPos);
	PlayerPacket* getSpecificPlayer(const RakNet::RakNetGUID& guid);
	SpellPacket* getSpecificSpell(const uint64_t& creatorGUID, const uint64_t& spellID);

	// Helper funcs
	void handleRespawns(const uint32_t& diff);
	
	void handleCountdown(const uint32_t& diff);
	void countdownExecutionLogic();

	void handleRoundTime(const uint32_t& diff);
	void roundTimeExecutionLogic();

	void handleEndGameStateTime(const uint32_t& diff);
	void endGameTimeExecutionLogic();

private:
	RakNet::RakPeerInterface* m_serverPeer = nullptr;
	std::thread m_processThread;
	bool m_shutdownServer;
	std::mutex m_cleanupMutex;
	std::vector<PlayerPacket> m_connectedPlayers;
	std::vector<Respawner> m_respawnList;
	std::unordered_map<uint64_t, std::vector<SpellPacket>> m_activeSpells;

	ServerInfo m_serverInfo;
	bool m_initialized = false;
	RakNet::RakNetGUID m_adminID;
	uint32_t m_countdown;
	uint32_t m_roundTimer;

	TimedCallback m_timedRunTimer;
	TimedCallback m_timedCountdownTimer;
	TimedCallback m_timedGameInEndStateTimer;

};

#endif
