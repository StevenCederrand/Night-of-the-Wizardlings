#ifndef _CLIENT_H
#define _CLIENT_H
#include <Pch/Pch.h>
#include <Spells/Spell.h>
#include "NetworkPlayers.h"
#include "NetworkSpells.h"

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
	void createSpellOnNetwork(Spell& spell);
	void updateSpellOnNetwork(Spell& spell);
	void destroySpellOnNetwork(Spell& spell);
	void updateNetworkEntities(const float& dt);
	void sendStartRequestToServer();
	const std::vector<std::pair<unsigned int, ServerInfo>>& getServerList() const;
	const std::vector<PlayerPacket>& getConnectedPlayers() const;
	
	NetworkPlayers& getNetworkPlayersREF();
	NetworkSpells& getNetworkSpellsREF();

	const std::vector<SpellPacket>& getNetworkSpells();
	void refreshServerList();
	bool doneRefreshingServerList();
	const bool& isInitialized() const;

	const ServerInfo& getServerByID(const unsigned int& ID) const;
	const bool doesServerExist(const unsigned int& ID) const;

	const bool& isConnectedToSever() const;
	const bool& connectionFailed() const;

private:

	void updateDataOnServer();
	void findAllServerAddresses();
	unsigned char getPacketID(RakNet::Packet* p);
	SpellPacket* findActiveSpell(const SpellPacket& packet);
	NetworkSpells::SpellEntity* findSpellEntityInNetworkSpells(const SpellPacket& packet);
	void removeActiveSpell(const SpellPacket& packet);
	void printAllConnectedPlayers();

private:
	RakNet::RakPeerInterface* m_clientPeer;
	RakNet::SystemAddress m_serverAddress;
	std::vector<std::pair<unsigned int, ServerInfo>> m_serverList;
	bool m_isRefreshingServerList;
	bool m_isConnectedToAnServer;
	bool m_failedToConnect;
	bool m_serverOwner;

	std::thread m_processThread;
	bool m_shutdownThread;
	bool m_initialized = false;
	
	std::vector<PlayerPacket> m_connectedPlayers;
	PlayerPacket m_playerData;
	NetworkPlayers m_playerEntities;
	NetworkSpells m_spellEntities;
	std::mutex m_cleanupMutex;
	

	//std::unordered_map<uint64_t, std::vector<SpellPacket>> m_activeSpells;
	std::vector<SpellPacket> m_activeSpells;
	std::vector<SpellPacket> m_spellQueue;

};

#endif