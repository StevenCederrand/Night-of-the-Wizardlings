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

	struct PickupSpawnLocation {
		char name[16];
		glm::vec3 position;
	};

	struct BuffedPlayer {
		PlayerPacket* player;
		uint32_t currentTime = NetGlobals::damageBuffActiveTimeMS;
	};

private:
	unsigned char getPacketID(RakNet::Packet* p);
	bool handleLostPlayer(const RakNet::Packet& packet, const RakNet::BitStream& bsIn);
	void stateChange(NetGlobals::SERVER_STATE newState);
	void sendStreamToAllClients(RakNet::BitStream& stream, PacketReliability flag = RELIABLE_ORDERED);

	// Collision with players
	void handleCollisionWithSpells(HitPacket* hitpacket, SpellPacket* spell, PlayerPacket* shooter, PlayerPacket* target);
	bool validDeflect(SpellPacket* spell, PlayerPacket* target);
	bool specificSpellCollision(const SpellPacket& spellPacket, const glm::vec3& playerPos, const std::vector<glm::vec3>& axis);
	glm::vec3 OBBclosestPoint(const SpellPacket& spellPacket, const std::vector<glm::vec3>& axis, const glm::vec3& playerPos);
	
	PlayerPacket* getSpecificPlayer(const RakNet::RakNetGUID& guid);
	SpellPacket* getSpecificSpell(const uint64_t& creatorGUID, const uint64_t& spellID);

	void checkCollisionBetweenPlayersAndPickups();
	bool isCollidingWithPickup(const PlayerPacket& player, const PickupPacket& pickup);

	// Helper funcs
	void updatePlayersWithDamageBuffs(const uint32_t& diff);

	void handleRespawns(const uint32_t& diff);
	void hardRespawnPlayer(PlayerPacket& player);
	void resetScores();
	void respawnPlayers();
	void resetPlayerBuffs();
	void removePlayerBuff(const PlayerPacket* player);

	void handleCountdown(const uint32_t& diff);
	void countdownExecutionLogic();

	void handleRoundTime(const uint32_t& diff);
	void roundTimeExecutionLogic();

	void handleEndGameStateTime(const uint32_t& diff);
	void endGameTimeExecutionLogic();

	void handlePickupTimer(const uint32_t& diff);
	void spawnPickup();
	void notifyPickup();
	bool gameAlmostFinished();

	void removeUnusedObjects_routine();
	void m_updateClientsWithServertime();
	void resetServerData();

	void createPickupSpawnLocations();
	void createPlayerSpawnLocations();
	void destroyPickupOverNetwork(PickupPacket& pickupPacket);
	void copyStringToCharArray(char Dest[16], std::string Src);
	void copyCharArrayOver(char Dest[16], char Src[16]);
	void destroyAllPickups();
	PickupType getRandomPickupType();
	PickupSpawnLocation* getRandomPickupSpawnLocation();
	const glm::vec3& getRandomSpawnLocation();

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
	TimedCallback m_timedUnusedObjectRemoval;
	TimedCallback m_timedPickupSpawner;
	TimedCallback m_updateClientsWithServertimeTimer;;

	
	uint64_t m_pickupID;
	bool m_pickupNotified;
	std::vector<PickupSpawnLocation> m_pickupSpawnLocations;
	std::vector<PickupPacket> m_activePickups;
	std::vector<PickupPacket> m_queuedPickups;
	std::vector<BuffedPlayer> m_buffedPlayers;
	std::vector<glm::vec3> m_playerSpawnLocations;

};

#endif
