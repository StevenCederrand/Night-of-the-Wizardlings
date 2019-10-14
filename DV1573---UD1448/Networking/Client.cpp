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
			logTrace("[CLIENT] 1 !\n");
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			size_t nrOfConnectedPlayers;
			bsIn.Read(nrOfConnectedPlayers);

			for (size_t i = 0; i < nrOfConnectedPlayers; i++) {
				PlayerPacket player;
				player.Serialize(false, bsIn);
				m_connectedPlayers.emplace_back(player);

				std::lock_guard<std::mutex> lockGuard(NetGlobals::gameSyncMutex);
				NetworkPlayers::PlayerEntity* pE = new NetworkPlayers::PlayerEntity();

				pE->data = player;
				pE->flag = NetworkPlayers::FLAG::ADD;
				pE->gameobject = nullptr;
				m_playerEntities.m_players.emplace_back(pE);

			}
			printAllConnectedPlayers();

		}
		break;
		case PLAYER_JOINED:
		{
			logTrace("New player joined!");
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			PlayerPacket player;
			player.Serialize(false, bsIn);
			m_connectedPlayers.emplace_back(player);


			std::lock_guard<std::mutex> lockGuard(NetGlobals::gameSyncMutex);
			NetworkPlayers::PlayerEntity* pE = new NetworkPlayers::PlayerEntity();

			pE->data = player;
			pE->flag = NetworkPlayers::FLAG::ADD;
			pE->gameobject = nullptr;
			m_playerEntities.m_players.emplace_back(pE);

			printAllConnectedPlayers();

		}
		break;
		case PLAYER_DISCONNECTED:
		{
			logTrace("[CLIENT] 1 !\n");
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			RakNet::AddressOrGUID guidOfDisconnectedPlayer;
			bsIn.Read(guidOfDisconnectedPlayer);

			bool found = false;
			for (size_t i = 0; i < m_connectedPlayers.size() && !found; i++) {
				if (guidOfDisconnectedPlayer == m_connectedPlayers[i].guid) {

					if (m_playerEntities.m_players[i]->data.guid == guidOfDisconnectedPlayer)
					{
						m_playerEntities.m_players[i]->flag = NetworkPlayers::FLAG::REMOVE;
					}
					else
					{
						logError("Well shit, client thread and main thread is not synched up, so the removal of a client is not going to happen..");
					}

					m_connectedPlayers.erase(m_connectedPlayers.begin() + i);
					found = true;
				}
			}


			printAllConnectedPlayers();
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
				//logTrace("GUID: {0}, Looking for guid: {1}",pData.guid.ToString(), m_connectedPlayers[i]->getData().guid.ToString());
				if (m_connectedPlayers[i].guid == pData.guid)
				{
					m_connectedPlayers[i] = pData;
					/*logTrace("Updated player with GUID: {0}, new position: {1}, {2}, {3}", m_connectedPlayers[i]->m_data.guid.ToString(),
						m_connectedPlayers[i]->m_data.position.x,
						m_connectedPlayers[i]->m_data.position.y,
						m_connectedPlayers[i]->m_data.position.z);*/

					if (m_playerEntities.m_players[i]->data.guid == pData.guid)
					{
						m_playerEntities.m_players[i]->data = pData;
					}
					else {
						logError("Well fuck, now the update between the server and client is not synched up with the main thread, let's hope that this message never shows!");
					}

					break;
				}
			}

		}
		break;
		case ADMIN_PACKET:
		{
			m_serverOwner = true;
			logTrace("[CLIENT-ADMIN]Press E to start game!");
		}
		break;
		case SERVER_CURRENT_STATE:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			ServerStateChange stateChange;
			stateChange.Serialize(false, bsIn);

			if (stateChange.currentState == NetGlobals::ServerState::GameStarted) {
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
			logTrace("[CLIENT] Added spell to spell list for player {0}, spell ID {1}", spellPacket.CreatorGUID.ToString(), spellPacket.SpellID);
			NetworkSpells::SpellEntity se;

			se.spellData = spellPacket;
			se.flag = NetworkSpells::FLAG::ADD;
			se.tempObject = nullptr;
			std::lock_guard<std::mutex> lockGuard(NetGlobals::gameSyncMutex);
			m_spellEntities.m_entities.emplace_back(se);
			
			m_activeSpells.emplace_back(spellPacket);
			
		/*	auto item = m_activeSpells.find(spellPacket.CreatorGUID.g);

			if (item == m_activeSpells.end()) {
				logTrace("[CLIENT] Created a new list with spells for player {0}, spell ID {1}", spellPacket.CreatorGUID.ToString(), spellPacket.SpellID);
				std::vector<SpellPacket> spellVec;
				spellVec.reserve(10);
				spellVec.emplace_back(spellPacket);
				m_activeSpells[spellPacket.CreatorGUID.g] = spellVec;
			}
			else {
				logTrace("[CLIENT] Added spell to spell list for player {0}, spell ID {1}", spellPacket.CreatorGUID.ToString(), spellPacket.SpellID);
				item._Ptr->_Myval.second.emplace_back(spellPacket);
			}
			*/
		}

		break;
		
		case SPELL_UPDATE:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			SpellPacket spellPacket;
			spellPacket.Serialize(false, bsIn);
			std::lock_guard<std::mutex> lockGuard(NetGlobals::gameSyncMutex);
			//logTrace("[CLIENT] Updating spell");
			SpellPacket* sp = findActiveSpell(spellPacket);
			//logTrace("[CLIENT] {0}", spellPacket.toString());
			if (sp != nullptr) {
				sp->Position = spellPacket.Position;
				sp->Rotation = spellPacket.Rotation;
				
				NetworkSpells::SpellEntity* ne = findSpellEntityInNetworkSpells(spellPacket);
				if (ne != nullptr) {
					ne->spellData.Position = spellPacket.Position;
					ne->spellData.Rotation = spellPacket.Rotation;
				}
			}
			
			/*if (item != m_activeSpells.end()) {
				auto& spellVec = item._Ptr->_Myval.second;
				
				for (size_t i = 0; i < spellVec.size(); i++) {
					if (spellVec[i].SpellID == spellPacket.SpellID) {
						spellVec[i].Position = spellPacket.Position;
						spellVec[i].Rotation = spellPacket.Rotation;
					}
				}
				
			}*/
		}

		break;

		case SPELL_DESTROY:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			SpellPacket spellPacket;
			spellPacket.Serialize(false, bsIn);
			bsIn.SetReadOffset(0);
			logTrace("[CLIENT] SPELL DESTROYED");
			
			std::lock_guard<std::mutex> lockGuard(NetGlobals::gameSyncMutex);
			
			NetworkSpells::SpellEntity* ne = findSpellEntityInNetworkSpells(spellPacket);
			if(ne != nullptr)
				ne->flag = NetworkSpells::FLAG::REMOVE;
			
			
			removeActiveSpell(spellPacket);
			//auto item = m_activeSpells.find(spellPacket.CreatorGUID.g);

		/*	if (item != m_activeSpells.end()) {
				auto& spellVec = item._Ptr->_Myval.second;
				bool deleted = false;
				for (size_t i = 0; i < spellVec.size() && !deleted; i++) {
					if (spellVec[i].SpellID == spellPacket.SpellID) {
						logTrace("[CLIENT] Deleted a spell with spell ID: {0} from client {1}", spellVec[i].SpellID, spellVec[i].CreatorGUID.ToString());
						spellVec.erase(spellVec.begin() + i);
						deleted = true;
					}
				}

				if (spellVec.size() == 0)
				{
					logTrace("[CLIENT] Deleted the local spell container for client with ID {0}", spellPacket.CreatorGUID.ToString());
					m_activeSpells.erase(item);
				}

			}*/

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
	m_playerData.health = player->getHealth();
	m_playerData.position = player->getPlayerPos();
	m_playerData.rotation = glm::vec3(
		player->getCamera()->getViewMat()[1][0] - glm::radians(90.0f),
		player->getCamera()->getViewMat()[1][1] - glm::radians(90.0f),
		player->getCamera()->getViewMat()[1][2] - glm::radians(90.0f)
	);
}

// Note to self: Guid creation is slow af, possible solution is to have some sort of queue with pointers to the spells so
// that the client thread can set the GUID of the spell within its own loop. Otherwise the main thread will stop for a brief and
// very noticable moment.
void Client::createSpellOnNetwork(Spell& spell)
{
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
		stateChange.currentState = NetGlobals::ServerState::GameStarted;
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
	logTrace("Fetching server list...");
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

/* When we have a UI this will most likely be threaded */
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
					if (info.currentState == NetGlobals::ServerState::GameStarted) continue;

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
	//logTrace("[CLIENT] Finding active spell");
	for (size_t i = 0; i < m_activeSpells.size(); i++) {
		SpellPacket& sp = m_activeSpells[i];

		if (sp.CreatorGUID == packet.CreatorGUID && sp.SpellID == packet.SpellID) {
			//logTrace("[CLIENT] Returning active spell");
			return &sp;
		}
	}

	return nullptr;
}

NetworkSpells::SpellEntity* Client::findSpellEntityInNetworkSpells(const SpellPacket& packet)
{
	//logTrace("[CLIENT] Finding spell entity");
	for (size_t i = 0; i < m_spellEntities.m_entities.size(); i++) {
		
		SpellPacket& sp = m_spellEntities.m_entities[i].spellData;
	
		if (sp.CreatorGUID == packet.CreatorGUID && sp.SpellID == packet.SpellID) {
			//logTrace("[CLIENT] Returning spell entity");
			return &m_spellEntities.m_entities[i];
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
			logTrace("[CLIENT] Deleted a spell with spell ID: {0} from client {1}", sp.SpellID, sp.CreatorGUID.ToString());
			return;
		}
			
	}
}

void Client::printAllConnectedPlayers()
{
	/*for (size_t i = 0; i < m_connectedPlayers.size(); i++) {
		logTrace("Client ({0})\n{3}\n{1}\n{2}", (i + 1), m_connectedPlayers[i]->toString(), "________________________", "________________________");
	}*/
}
