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
	void connectToAnotherServer(const ServerInfo& server, bool spectatorMode);
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
	void sendDestructionPacket(const DestructionPacket& destructionPacket);
	void sendReadyRequestToServer();
	
	void updateNetworkEntities(const float& dt);
	void refreshServerList();
	void startSendingUpdatePackages();
	void assignSpellHandler(SpellHandler* spellHandler);
	void setUsername(const std::string& userName);
	void spectateNext(); /* Only works if you're a spectator */
	void clearDestroyedWallsVector();


	const std::vector<std::pair<unsigned int, ServerInfo>>& getServerList() const;
	const std::vector<PlayerPacket>& getConnectedPlayers() const;
	const std::vector<SpellPacket>& getNetworkSpells();
	const PlayerPacket* getSpectatedPlayer() const;
	const ServerInfo& getServerByID(const unsigned int& ID) const;
	
	NetworkPlayers& getNetworkPlayersREF();
	NetworkSpells& getNetworkSpellsREF();
	
	const PlayerPacket& getMyData() const;
	const PlayerPacket* getLatestPlayerThatHitMe() const;
	const PlayerPacket* findPlayerWithGuid(const RakNet::AddressOrGUID guid);
	const ServerStateChange& getServerState() const;
	const CountdownPacket& getCountdownPacket() const;
	const CountdownPacket& getRespawnTime() const;
	const RoundTimePacket& getRoundTimePacket() const;
	const Evnt readNextEvent();
	const std::vector<DestructionPacket>& getDestructedWalls();

	const int& getNumberOfReadyPlayers() const;
	const int getNumberOfPlayers() const;

	const bool doneRefreshingServerList() const;
	const bool doesServerExist(const unsigned int& ID) const;
	const bool& isInitialized() const;
	const bool& isConnectedToSever() const;
	const bool& connectionFailed() const;
	const bool& isServerOwner() const;
	const bool& isSpectating() const;

	SpellHandler* getSpellhandler() const { return m_spellHandler;} // TODO: Delete later
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
	bool m_spectating;

	int m_numberOfReadyPlayers;

	size_t m_spectateIndex;

	PlayerPacket m_myPlayerDataPacket;
	PlayerPacket* m_latestPlayerThatHitMe;
	PlayerPacket* m_spectatedPlayer;
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
	std::vector<DestructionPacket> m_destructionQueue;

	std::vector<Evnt> m_playerEvents;
	std::vector<DestructionPacket> m_destroyedWalls;

	TimedCallback m_routineCleanupTimer;

};

#endif