#ifndef _CLIENT_H
#define _CLIENT_H
#include <Pch/Pch.h>
#include <Spells/Spell.h>
#include "NetworkPlayers.h"
#include "NetworkSpells.h"
#include "NetworkPickups.h"
#include "PlayerEvents.h"
#include <System/TimedCallback.h>

class Player;

class Client
{
public:
	Client();
	~Client();

	static Client* getInstance();

	void startup();
	void destroy();
	void connectToAnotherServer(const ServerInfo& server);
	void connectToMyServer();
	void ThreadedUpdate();
	void processAndHandlePackets();
	void updatePlayerData(Player* player);
	void createSpellOnNetwork(const Spell& spell);
	void updateSpellOnNetwork(const Spell& spell);
	void destroySpellOnNetwork(const Spell& spell);
	void requestToDestroyClientSpell(const SpellPacket& packet);
	void sendHitRequest(Spell& spell, NetworkPlayers::PlayerEntity& playerThatWasHit);
	void sendHitRequest(Spell& spell, const PlayerPacket& playerThatWasHit);
	void updateNetworkEntities(const float& dt);
	void sendStartRequestToServer();
	void refreshServerList();
	void startSendingUpdatePackages();
	void assignSpellHandler(SpellHandler* spellHandler);
	void setUsername(const std::string& userName);
	
	void updatePlayersMutexGuard();
	void updateSpellsMutexGuard();
	void updatePickupsMutexGuard();
	void eventMutexGuard();
	void cleanupMutexGuard();
	void deflectSpellsMutexGuard();
	void renderPickupNotificationsMutexGuard();
	void renderKillFeedMutexGuard();


	const std::vector<std::pair<unsigned int, ServerInfo>>& getServerList() const;
	const std::vector<PlayerPacket>& getConnectedPlayers() const;
	const std::vector<SpellPacket>& getNetworkSpells();
	const ServerInfo& getServerByID(const unsigned int& ID) const;
	
	NetworkPlayers& getNetworkPlayersREF();
	NetworkSpells& getNetworkSpellsREF();
	
	const PlayerPacket& getMyData() const;
	const PlayerPacket* getLatestPlayerThatHitMe() const;
	const ServerStateChange& getServerState() const;
	const CountdownPacket& getCountdownPacket() const;
	const CountdownPacket& getRespawnTime() const;
	const RoundTimePacket& getRoundTimePacket() const;
	const PlayerEvents readNextEvent();

	const bool doneRefreshingServerList() const;
	const bool doesServerExist(const unsigned int& ID) const;
	const bool& isInitialized() const;
	const bool& isConnectedToSever() const;
	const bool& connectionFailed() const;
	const bool& isServerOwner() const;
private:

	unsigned char getPacketID(RakNet::Packet* p);

	void updateDataOnServer();
	void findAllServerAddresses();
	void removeActiveSpell(const SpellPacket& packet);
	void removeConnectedPlayer(const RakNet::AddressOrGUID& guid);
	void resetPlayerData();
	void routineCleanup(); // this is a callback func for routineCallbackTimer

	SpellPacket* findActiveSpell(const SpellPacket& packet);
	PlayerPacket* findPlayerByGuid(const RakNet::AddressOrGUID& guid);
	NetworkSpells::SpellEntity* findSpellEntityInNetworkSpells(const SpellPacket& packet);
	NetworkPlayers::PlayerEntity* findPlayerEntityInNetworkPlayers(const RakNet::AddressOrGUID& guid);

private:
	RakNet::RakPeerInterface* m_clientPeer;
	RakNet::SystemAddress m_serverAddress;
	
	char m_userName[16] = { ' ' };
	
	bool m_inGame;
	bool m_isRefreshingServerList;
	bool m_isConnectedToAnServer;
	bool m_failedToConnect;
	bool m_serverOwner;
	bool m_shutdownThread;
	bool m_initialized = false;
	bool m_sendUpdatePackages;

	PlayerPacket m_myPlayerDataPacket;
	PlayerPacket* m_latestPlayerThatHitMe;
	ServerStateChange m_serverState;
	CountdownPacket m_countDownPacket;
	CountdownPacket m_respawnTime;
	RoundTimePacket m_roundTimePacket;
	
	ServerTimePacket m_serverTimePacket;

	NetworkPlayers m_networkPlayers;
	NetworkSpells m_networkSpells;
	NetworkPickups* m_networkPickup;
	
	SpellHandler* m_spellHandler;

	std::thread m_processThread;

	std::vector<std::pair<unsigned int, ServerInfo>> m_serverList;
	std::vector<PlayerPacket> m_connectedPlayers;
	std::vector<SpellPacket> m_activeSpells;
	std::vector<HitPacket> m_spellsHitQueue;
	std::vector<SpellPacket> m_updateSpellQueue;
	std::vector<SpellPacket> m_removeOrAddSpellQueue;
	std::vector<SpellPacket> m_removalOfClientSpellsQueue;
	std::vector<PlayerEvents> m_playerEvents;
	
	std::mutex m_cleanupMutex;
	std::mutex m_updatePickupsMutex;
	std::mutex m_updatePlayersMutex;
	std::mutex m_updateSpellsMutex;
	std::mutex m_playerEventMutex;
	std::mutex m_deflectSpellMutex;
	std::mutex m_renderPickupNotificationMutex;
	std::mutex m_renderKillFeedMutex;

	TimedCallback m_routineCleanupTimer;

};

#endif