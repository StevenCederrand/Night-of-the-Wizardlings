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
	void refreshServerList();
	
	const std::vector<std::pair<unsigned int, ServerInfo>>& getServerList() const;
	const std::vector<PlayerPacket>& getConnectedPlayers() const;
	const std::vector<SpellPacket>& getNetworkSpells();
	const ServerInfo& getServerByID(const unsigned int& ID) const;
	
	NetworkPlayers& getNetworkPlayersREF();
	NetworkSpells& getNetworkSpellsREF();

	const bool doneRefreshingServerList() const;
	const bool doesServerExist(const unsigned int& ID) const;
	const bool& isInitialized() const;
	const bool& isConnectedToSever() const;
	const bool& connectionFailed() const;

private:
	
	unsigned char getPacketID(RakNet::Packet* p);

	void updateDataOnServer();
	void findAllServerAddresses();
	
	void removeActiveSpell(const SpellPacket& packet);
	void removeConnectedPlayer(const RakNet::AddressOrGUID& guid);
	
	SpellPacket* findActiveSpell(const SpellPacket& packet);
	
	NetworkSpells::SpellEntity* findSpellEntityInNetworkSpells(const SpellPacket& packet);
	NetworkPlayers::PlayerEntity* findPlayerEntityInNetworkPlayers(const RakNet::AddressOrGUID& guid);

private:
	RakNet::RakPeerInterface* m_clientPeer;
	RakNet::SystemAddress m_serverAddress;
	std::vector<std::pair<unsigned int, ServerInfo>> m_serverList;

	bool m_isRefreshingServerList;
	bool m_isConnectedToAnServer;
	bool m_failedToConnect;
	bool m_serverOwner;

	bool m_shutdownThread;
	bool m_initialized = false;

	std::thread m_processThread;
	
	PlayerPacket m_myPlayerDataPacket;
	
	std::vector<PlayerPacket> m_connectedPlayers;
	NetworkPlayers m_networkPlayers;
	NetworkSpells m_networkSpells;
	
	std::mutex m_cleanupMutex;
	
	std::vector<SpellPacket> m_activeSpells;
	std::vector<SpellPacket> m_spellQueue;

};

#endif