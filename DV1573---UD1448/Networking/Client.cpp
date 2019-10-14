#include <Pch/Pch.h>
#include "Client.h"
#include <Player/Player.h>

Client::Client()
{
}

Client::~Client()
{
}

Client* Client::getInstance()
{
	static Client c;
	return &c;
}

void Client::startup()
{
	if (!m_initialized) {
		m_spellQueue.reserve(250);
		m_connectedPlayers.reserve(NetGlobals::MaximumConnections);
		m_clientPeer = RakNet::RakPeerInterface::GetInstance();
		m_clientPeer->Startup(1, &RakNet::SocketDescriptor(), 1);
		m_initialized = true;	
	}
}

void Client::destroy()
{
	if (m_initialized) {
		
		// In it's own scope	
		{
			std::lock_guard<std::mutex> lockGuard(m_cleanupMutex);
			m_shutdownThread = true;
		}

		logTrace("Waiting for client thread to finish...");
		if(m_processThread.joinable())
			m_processThread.join();
		logTrace("Client process thread shutdown");
		
		m_initialized = false;
		RakNet::RakPeerInterface::DestroyInstance(m_clientPeer);


	}
}

void Client::connectToAnotherServer(const ServerInfo& server)
{
	m_failedToConnect = false;
	m_shutdownThread = false;
	m_isConnectedToAnServer = false;
	m_serverOwner = false;

	bool status = m_clientPeer->Connect(server.serverAddress.ToString(false), server.serverAddress.GetPort(), 0, 0, 0) == RakNet::CONNECTION_ATTEMPT_STARTED;
	assert((status == true, "Client connecting to {0} failed!", server.serverName));

	if (m_processThread.joinable())
		m_processThread.join();

	m_clientPeer->SetTimeoutTime(NetGlobals::timeoutTimeMS, server.serverAddress);
	m_processThread = std::thread(&Client::ThreadedUpdate, this);
	
}

void Client::connectToMyServer()
{
	m_failedToConnect = false;
	m_shutdownThread = false;
	m_isConnectedToAnServer = false;
	m_serverOwner = false;

	bool status = m_clientPeer->Connect("localhost", NetGlobals::ServerPort, 0, 0, 0) == RakNet::CONNECTION_ATTEMPT_STARTED;
	assert((status == true, "Client connecting to localhost failed!"));
	
	if (m_processThread.joinable())
		m_processThread.join();

	
	m_processThread = std::thread(&Client::ThreadedUpdate, this);
}

void Client::ThreadedUpdate()
{
	bool clientRunning = true;
	

	while (clientRunning)
	{	
		
		processAndHandlePackets();
		updateDataOnServer();
		
		// Scope
		{
			std::lock_guard<std::mutex> lockGuard(m_cleanupMutex);
			if (m_shutdownThread == true)
				clientRunning = false;
		}
	
		RakSleep(NetGlobals::threadSleepTime);
	}

	// Client has been told to shutdown here so send a disconnection packet if you're still connected
	if (m_isConnectedToAnServer)
	{
		logTrace("[CLIENT] Sent a disconnect package to server :)");
		RakNet::BitStream stream;
		stream.Write((RakNet::MessageID)ID_DISCONNECTION_NOTIFICATION);
		m_clientPeer->Send(&stream,IMMEDIATE_PRIORITY , RELIABLE_ORDERED, 0,m_serverAddress, false);
		RakSleep(250);
	}

}

void Client::processAndHandlePackets()
{
	for (RakNet::Packet* packet = m_clientPeer->Receive(); packet; m_clientPeer->DeallocatePacket(packet), packet = m_clientPeer->Receive())
	{

		RakNet::BitStream bsOut;
		RakNet::BitStream bsIn(packet->data, packet->length, false);

		auto packetID = getPacketID(packet);

		switch (packetID)
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			logTrace("[CLIENT] Connected to server but not sure if actually accepted to the server.\n");
		}
		break;

		case ID_CONNECTION_ATTEMPT_FAILED:
			logTrace("[CLIENT] Connection failed, server might be full.\n");
			m_isConnectedToAnServer = false;
			m_failedToConnect = true;
			m_shutdownThread = true;
			break;

		case ID_ALREADY_CONNECTED:
			logTrace("[CLIENT] You are already connected to the server\n");
			break;

		case ID_CONNECTION_BANNED:
			logTrace("[CLIENT] You are banned for that server.\n");
			break;

		case ID_INVALID_PASSWORD:
			logTrace("[CLIENT] Invalid server password.\n");
			break;

		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			m_shutdownThread = true;
			logTrace("[CLIENT] Client Error: incompatible protocol version!\n");
			break;

		case ID_NO_FREE_INCOMING_CONNECTIONS:
			logTrace("[CLIENT] Client Error: No free incoming connection slots!\n");
			m_failedToConnect = true;
			m_isConnectedToAnServer = false;
			m_shutdownThread = true;
			break;

		case ID_DISCONNECTION_NOTIFICATION:
			logTrace("[CLIENT] Disconnected from server!\n");
			m_failedToConnect = true;
			m_isConnectedToAnServer = false;
			m_shutdownThread = true;
			break;

		case ID_CONNECTION_LOST:
			logTrace("[CLIENT] Connection to the server is lost!\n");
			m_isConnectedToAnServer = false;
			m_shutdownThread = true;
			break;

		case PLAYER_ACCEPTED_TO_SERVER:
		{
			logTrace("[CLIENT] Connected and accepted by server! Welcome!.\n");
			m_serverAddress = packet->systemAddress;
			m_isConnectedToAnServer = true;
			m_playerData.guid = m_clientPeer->GetMyGUID();
		}
			break;

		case INFO_ABOUT_OTHER_PLAYERS:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			size_t nrOfConnectedPlayers;
			bsIn.Read(nrOfConnectedPlayers);

			for (size_t i = 0; i < nrOfConnectedPlayers; i++) {
				PlayerPacket player;
				player.Serialize(false, bsIn);
				m_connectedPlayers.emplace_back(player);

				std::lock_guard<std::mutex> lockGuard(NetGlobals::gameSyncMutex);
				NetworkPlayers::PlayerEntity pE;

				pE.data = player;
				pE.flag = NetGlobals::THREAD_FLAG::ADD;
				pE.gameobject = nullptr;
				m_playerEntities.m_players.emplace_back(pE);

			}

		}
		break;
		case PLAYER_JOINED:
		{
			logTrace("New player joined!");
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			PlayerPacket player;
			player.Serialize(false, bsIn);
			m_connectedPlayers.emplace_back(player);


			std::lock_guard<std::mutex> lockGuard(m_playerEntities.m_mutex);
			NetworkPlayers::PlayerEntity pE;

			pE.data = player;
			pE.flag = NetGlobals::THREAD_FLAG::ADD;
			pE.gameobject = nullptr;
			m_playerEntities.m_players.emplace_back(pE);

		}
		break;
		case PLAYER_DISCONNECTED:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			RakNet::AddressOrGUID guidOfDisconnectedPlayer;
			bsIn.Read(guidOfDisconnectedPlayer);
			
			{
				std::lock_guard<std::mutex> lockGuard(m_playerEntities.m_mutex);
				NetworkPlayers::PlayerEntity* pE = findPlayerEntityInNetworkPlayers(guidOfDisconnectedPlayer);
				if (pE != nullptr)
					pE->flag = NetGlobals::THREAD_FLAG::REMOVE;
			}

			removeConnectedPlayer(guidOfDisconnectedPlayer);		
		}
		break;
		case PLAYER_UPDATE_PACKET:
		{
			//logTrace("Player data received from server");
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			PlayerPacket pData;
			pData.Serialize(false, bsIn);

			for (size_t i = 0; i < m_connectedPlayers.size(); i++)
			{
			
				if (m_connectedPlayers[i].guid == pData.guid)
				{
					m_connectedPlayers[i] = pData;
				
					if (m_playerEntities.m_players[i].data.guid == pData.guid)
					{
						m_playerEntities.m_players[i].data = pData;
					}
					else {
						logWarning("[CLIENT] Client skipped a update on a client due to sync problems. (Should resolve itself with time)");
					}

					break;
				}
			}

		}
		break;
		case ADMIN_PACKET:
		{
			m_serverOwner = true;
			logTrace("[CLIENT-ADMIN] Press E to start game!");
		}
		break;
		case SERVER_CURRENT_STATE:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			ServerStateChange stateChange;
			stateChange.Serialize(false, bsIn);

			if (stateChange.currentState == NetGlobals::SERVER_STATE::GAME_IN_SESSION) {
				logTrace("[SERVER->CLIENT]******** GAME STARTED ********");
			}
		}
		break;
		// Note, Can a thread have multiple mutexes at the same time? 
	
		case SPELL_CREATED:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			SpellPacket spellPacket;
			spellPacket.Serialize(false, bsIn);
			//logTrace("[CLIENT] Added spell to spell list for player {0}, spell ID {1}", spellPacket.CreatorGUID.ToString(), spellPacket.SpellID);
			NetworkSpells::SpellEntity se;

			se.spellData = spellPacket;
			se.flag = NetGlobals::THREAD_FLAG::ADD;
			se.gameobject = nullptr;
			
			{
				std::lock_guard<std::mutex> lockGuard(m_spellEntities.m_mutex);
				m_spellEntities.m_entities.emplace_back(se);
			}

			m_activeSpells.emplace_back(spellPacket);
			
		}

		break;
		
		case SPELL_UPDATE:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			SpellPacket spellPacket;
			spellPacket.Serialize(false, bsIn);

			for (size_t i = 0; i < m_activeSpells.size(); i++)
			{
				if (m_activeSpells[i].CreatorGUID == spellPacket.CreatorGUID &&
					m_activeSpells[i].SpellID == spellPacket.SpellID)
				{
					m_activeSpells[i].Position = spellPacket.Position;
					m_activeSpells[i].Rotation = spellPacket.Rotation;
				
					if (m_spellEntities.m_entities[i].spellData.CreatorGUID == spellPacket.CreatorGUID &&
						m_spellEntities.m_entities[i].spellData.SpellID == spellPacket.SpellID)
					{
						m_spellEntities.m_entities[i].spellData.Position = spellPacket.Position;
						m_spellEntities.m_entities[i].spellData.Rotation = spellPacket.Rotation;
					}
					else {
						logWarning("[CLIENT] Client skipped a update on a spell due to sync problems. (Should resolve itself with time)");
					}

					break;
				}
			}

		}

		break;

		case SPELL_DESTROY:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			SpellPacket spellPacket;
			spellPacket.Serialize(false, bsIn);
			bsIn.SetReadOffset(0);
			
			{
				std::lock_guard<std::mutex> lockGuard(m_spellEntities.m_mutex);
				NetworkSpells::SpellEntity* ne = findSpellEntityInNetworkSpells(spellPacket);
				if (ne != nullptr)
					ne->flag = NetGlobals::THREAD_FLAG::REMOVE;

			}

			removeActiveSpell(spellPacket);
		
		}
		break;

		case SPELL_PLAYER_HIT:
		{

		}
		break;

		default:
		{
			logWarning("[CLIENT] Unknown packet received!");
		}
		break;
		}
	}
}

void Client::updatePlayerData(Player* player)
{
	if (!m_initialized || !m_isConnectedToAnServer) return;

	m_playerData.health = player->getHealth();
	m_playerData.position = player->getPlayerPos();
	m_playerData.rotation = glm::vec3(
		player->getCamera()->getViewMat()[1][0] - glm::radians(90.0f),
		player->getCamera()->getViewMat()[1][1] - glm::radians(90.0f),
		player->getCamera()->getViewMat()[1][2] - glm::radians(90.0f)
	);
}

void Client::createSpellOnNetwork(Spell& spell)
{
	if (!m_initialized || !m_isConnectedToAnServer) return;

	SpellPacket spellPacket;
	spellPacket.packetType = SPELL_CREATED;
	spellPacket.CreatorGUID = m_clientPeer->GetMyGUID();
	spellPacket.Position = spell.getTransform().position;
	spellPacket.SpellID = spell.getUniqueID();
	spellPacket.Rotation = glm::vec3(0.0f);
	spellPacket.SpellType = SPELL_TYPE::UNKNOWN; // Type needs to be present in Spell class and not in sub classes.

	m_spellQueue.emplace_back(spellPacket);
}

void Client::updateSpellOnNetwork(Spell& spell)
{
	if (!m_initialized || !m_isConnectedToAnServer) return;

	SpellPacket spellPacket;
	spellPacket.packetType = SPELL_UPDATE;
	spellPacket.CreatorGUID = m_clientPeer->GetMyGUID();
	spellPacket.Position = spell.getTransform().position;
	spellPacket.SpellID = spell.getUniqueID();
	spellPacket.Rotation = glm::vec3(0.0f);
	spellPacket.SpellType = SPELL_TYPE::UNKNOWN; // Type needs to be present in Spell class and not in sub classes.

	m_spellQueue.emplace_back(spellPacket);
}

void Client::destroySpellOnNetwork(Spell& spell)
{
	if (!m_initialized || !m_isConnectedToAnServer) return;

	SpellPacket spellPacket;
	spellPacket.packetType = SPELL_DESTROY;
	spellPacket.CreatorGUID = m_clientPeer->GetMyGUID();
	spellPacket.Position = spell.getTransform().position;
	spellPacket.SpellID = spell.getUniqueID();
	spellPacket.Rotation = glm::vec3(0.0f);
	spellPacket.SpellType = SPELL_TYPE::UNKNOWN; // Type needs to be present in Spell class and not in sub classes.

	m_spellQueue.emplace_back(spellPacket);
}

void Client::updateNetworkEntities(const float& dt)
{
	if (m_initialized && m_isConnectedToAnServer) {
		m_playerEntities.update(dt);
		m_spellEntities.update(dt);
	}

}

void Client::sendStartRequestToServer()
{
	if (m_serverOwner) {
		RakNet::BitStream stream;
		stream.Write((RakNet::MessageID)SERVER_CHANGE_STATE);
		ServerStateChange stateChange;
		stateChange.currentState = NetGlobals::SERVER_STATE::GAME_IN_SESSION;
		stateChange.Serialize(true, stream);
		m_clientPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
	}
}

void Client::updateDataOnServer()
{
	// Player data sent to server
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)PLAYER_UPDATE_PACKET);
	m_playerData.Serialize(true, bsOut);
	m_clientPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);

	// Empty out the spell queue
	for (size_t i = 0; i < m_spellQueue.size(); i++) {
	
		RakNet::BitStream bsOut;
		bsOut.Write(m_spellQueue[i].packetType);
		m_spellQueue[i].Serialize(true, bsOut);
		m_clientPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
	}

	// Empty it
	m_spellQueue.clear();
}


const std::vector<std::pair<unsigned int, ServerInfo>>& Client::getServerList() const
{
	return m_serverList;
}

const std::vector<PlayerPacket>& Client::getConnectedPlayers() const
{
	return m_connectedPlayers;
}

NetworkPlayers& Client::getNetworkPlayersREF()
{
	return m_playerEntities;
}

NetworkSpells& Client::getNetworkSpellsREF()
{
	return m_spellEntities;
}

const std::vector<SpellPacket>& Client::getNetworkSpells()
{
	return m_activeSpells;
}

void Client::refreshServerList()
{
	logTrace("[CLIENT] Fetching server list...");
	m_isRefreshingServerList = true;
	m_serverList.clear();
	findAllServerAddresses();
}

bool Client::doneRefreshingServerList()
{
	return !m_isRefreshingServerList;
}

const bool& Client::isInitialized() const
{
	return m_initialized;
}

const ServerInfo& Client::getServerByID(const unsigned int& ID) const
{
	for (size_t i = 0; i < m_serverList.size(); i++)
	{
		if (m_serverList[i].first == ID)
			return m_serverList[i].second;
	}

	// Really ugly way to get rid of an compiler warning
	ServerInfo* c = nullptr;
	return *c;
}

const bool Client::doesServerExist(const unsigned int& ID) const
{
	for (size_t i = 0; i < m_serverList.size(); i++)
	{
		if (m_serverList[i].first == ID)
			return true;
	}
	return false;
}

const bool& Client::isConnectedToSever() const
{
	return m_isConnectedToAnServer;
}

const bool& Client::connectionFailed() const
{
	return m_failedToConnect;
}

void Client::findAllServerAddresses()
{
	m_clientPeer->Ping("255.255.255.255", NetGlobals::ServerPort, false);
	unsigned int ID = 0;
	auto searchTime = RakNet::GetTimeMS() + 0.25 * 1000; 
	while (RakNet::GetTimeMS() < searchTime)
	{
		for (RakNet::Packet* packet = m_clientPeer->Receive(); packet; m_clientPeer->DeallocatePacket(packet), packet = m_clientPeer->Receive())
		{
			auto packetID = getPacketID(packet);
			
			switch (packetID)
			{
				case ID_UNCONNECTED_PONG:
				{
					ServerInfo info;

					auto byteOffset = packet->length - static_cast<unsigned int>(sizeof(ServerInfo));

					RakNet::BitStream bsIn(packet->data + byteOffset, packet->length - byteOffset, false);
					info = *(ServerInfo*)bsIn.GetData();

					// If the pinged server is full or in session then don't add it to the server list
					if (info.connectedPlayers >= info.maxPlayers) continue;
					if (info.currentState == NetGlobals::SERVER_STATE::GAME_IN_SESSION) continue;

					info.serverAddress = packet->systemAddress;
					m_serverList.emplace_back(std::make_pair(ID++, info));
				}
				break;
			}
		}
	}

	m_isRefreshingServerList = false;

}

unsigned char Client::getPacketID(RakNet::Packet* p)
{
	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	else
		return (unsigned char)p->data[0];
}

SpellPacket* Client::findActiveSpell(const SpellPacket& packet)
{
	for (size_t i = 0; i < m_activeSpells.size(); i++) {
		SpellPacket& sp = m_activeSpells[i];

		if (sp.CreatorGUID == packet.CreatorGUID && sp.SpellID == packet.SpellID) {
			return &sp;
		}
	}

	return nullptr;
}

NetworkSpells::SpellEntity* Client::findSpellEntityInNetworkSpells(const SpellPacket& packet)
{
	for (size_t i = 0; i < m_spellEntities.m_entities.size(); i++) {
		
		SpellPacket& sp = m_spellEntities.m_entities[i].spellData;
	
		if (sp.CreatorGUID == packet.CreatorGUID && sp.SpellID == packet.SpellID) {
			return &m_spellEntities.m_entities[i];
		}
	}

	return nullptr;
}

NetworkPlayers::PlayerEntity* Client::findPlayerEntityInNetworkPlayers(const RakNet::AddressOrGUID& guid)
{
	for (size_t i = 0; i < m_playerEntities.m_players.size(); i++) {

		PlayerPacket& p = m_playerEntities.m_players[i].data;

		if (p.guid == guid) {
			return &m_playerEntities.m_players[i];
		}
	}

	return nullptr;
}

void Client::removeActiveSpell(const SpellPacket& packet)
{
	for (size_t i = 0; i < m_activeSpells.size(); i++) {
		SpellPacket& sp = m_activeSpells[i];

		if (sp.CreatorGUID == packet.CreatorGUID && sp.SpellID == packet.SpellID) {
			m_activeSpells.erase(m_activeSpells.begin() + i);
			return;
		}
			
	}
}

void Client::removeConnectedPlayer(const RakNet::AddressOrGUID& guid)
{
	for (size_t i = 0; i < m_connectedPlayers.size(); i++) {
		PlayerPacket& p = m_connectedPlayers[i];

		if (p.guid == guid) {
			m_connectedPlayers.erase(m_connectedPlayers.begin() + i);
			return;
		}

	}
}
