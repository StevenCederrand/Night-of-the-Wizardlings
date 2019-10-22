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
		m_updateSpellQueue.reserve(250);
		m_spellsHitQueue.reserve(250);
		m_removeOrAddSpellQueue.reserve(250);
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

		if (m_processThread.joinable()) {
			m_processThread.join();
		}

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
	assert((status == true, "[CLIENT] Client connecting to {0} failed!", server.serverName));

	if (m_processThread.joinable()) {
		m_processThread.join();
	}

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
	
	if (m_processThread.joinable()) {
		m_processThread.join();
	}
	
	m_processThread = std::thread(&Client::ThreadedUpdate, this);
}

void Client::ThreadedUpdate()
{
	bool clientRunning = true;
	
	while (clientRunning)
	{	
		
		processAndHandlePackets();
		/* Sends my data (such as my position, my spells) to the server and to 
		   all the other clients */
		updateDataOnServer();
		
		/* Checking for cleanups, it's in its own scope due to the lock guard
		   releases the mutex upon destruction. */ 
		{
			std::lock_guard<std::mutex> lockGuard(m_cleanupMutex);
			if (m_shutdownThread == true)
				clientRunning = false;
		}
		
		/* Put the thread to sleep, the tick rate is in NetGlobals class */
		RakSleep(NetGlobals::threadSleepTime);
	}

	// Client has been told to shutdown here so send a disconnection packet if you're still connected
	if (m_isConnectedToAnServer)
	{
		logTrace("[CLIENT] Sent a disconnect package to server :)");
		RakNet::BitStream stream;
		stream.Write((RakNet::MessageID)ID_DISCONNECTION_NOTIFICATION);
		m_clientPeer->Send(&stream, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
		RakSleep(250);
	}

}

void Client::processAndHandlePackets()
{
	/* This is the meat of the client, processing every packet that is coming in and sending packets to the server respectively */

	for (RakNet::Packet* packet = m_clientPeer->Receive(); packet; m_clientPeer->DeallocatePacket(packet), packet = m_clientPeer->Receive())
	{
		/* Construct the bitstream with the data from the packet */
		RakNet::BitStream bsIn(packet->data, packet->length, false);

		/* Get the ID of the packet so we know which packet it is */
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
			/* The server accepted you which means that the server had room for you & wasn't in session */
			logTrace("[CLIENT] Connected and accepted by server! Welcome!.\n");
			m_serverAddress = packet->systemAddress;
			m_isConnectedToAnServer = true;
			m_myPlayerDataPacket.guid = m_clientPeer->GetMyGUID();
		}
			break;

		case INFO_ABOUT_OTHER_PLAYERS:
		{
			/* You will get this package upon joining and getting accepted to a server. 
			   this is information or "PlayerPackets" of every player that already was present 
			   on the server before you. */

			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			size_t nrOfConnectedPlayers;
			bsIn.Read(nrOfConnectedPlayers);

			for (size_t i = 0; i < nrOfConnectedPlayers; i++) {
				PlayerPacket player;
				player.Serialize(false, bsIn);
				m_connectedPlayers.emplace_back(player);

				NetworkPlayers::PlayerEntity pE;

				pE.data = player;
				pE.flag = NetGlobals::THREAD_FLAG::ADD;
				pE.gameobject = nullptr;
				
				/* Thread lock guard because this needs to be synced with the main game thread because
				   this thread will add data to a list that is created and present in the main thread.

				   it's in its own scope because the mutex is released upon destruction
				   (whenever the lock guard goes out of scope.). This will make it synced &
				   avoid deadlocks. */
				{
					std::lock_guard<std::mutex> lockGuard(NetGlobals::gameSyncMutex);
					m_networkPlayers.m_players.emplace_back(pE);
				}
			}

		}
		break;

		case SPELL_ALL_EXISTING_SPELLS:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			size_t nrOfSpellsInGame;
			bsIn.Read(nrOfSpellsInGame);
			logTrace("[CLIENT] Got packet with all the existing spells");
			for (size_t i = 0; i < nrOfSpellsInGame; i++) {
				SpellPacket spellPacket;
				spellPacket.Serialize(false, bsIn);
				NetworkSpells::SpellEntity se;

				se.spellData = spellPacket;
				se.flag = NetGlobals::THREAD_FLAG::ADD;
				se.gameobject = nullptr;

				{
					std::lock_guard<std::mutex> lockGuard(m_networkSpells.m_mutex);
					m_networkSpells.m_entities.emplace_back(se);
				}

				m_activeSpells.emplace_back(spellPacket);

			}
		}

		break;

		case PLAYER_JOINED:
		{
			/* This is called whenever a player is joining the server, wheter it's your server or someone else's.
			   This works just as "INFO_ABOUT_OTHER_PLAYERS". The reason why there is two different ID's that does the same
			   thing is that, whenever a client joins a server he/she needs to know about ALL the players that are present on the server before
			   doing anything else. This one is just called whenever a player joins the server. */
			logTrace("New player joined!");
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			PlayerPacket player;
			player.Serialize(false, bsIn);
			m_connectedPlayers.emplace_back(player);


			std::lock_guard<std::mutex> lockGuard(m_networkPlayers.m_mutex);
			NetworkPlayers::PlayerEntity pE;

			pE.data = player;
			pE.flag = NetGlobals::THREAD_FLAG::ADD;
			pE.gameobject = nullptr;
			m_networkPlayers.m_players.emplace_back(pE);

		}
		break;
		case PLAYER_DISCONNECTED:
		{
			/* Again, thread synchronization because this thread is modifying something in another thread so a lock guard is used here.
			   This package is pretty self explanatory. It's whenever someone disconnected the server so in here the client removes 
			   the disconnected player from the internal list & flags the main thread that the object over there should be removed and cleaned up. */

			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			RakNet::AddressOrGUID guidOfDisconnectedPlayer;
			bsIn.Read(guidOfDisconnectedPlayer);
			
			{
				std::lock_guard<std::mutex> lockGuard(m_networkPlayers.m_mutex);
				NetworkPlayers::PlayerEntity* pE = findPlayerEntityInNetworkPlayers(guidOfDisconnectedPlayer);
				if (pE != nullptr)
					pE->flag = NetGlobals::THREAD_FLAG::REMOVE;
			}

			removeConnectedPlayer(guidOfDisconnectedPlayer);		
		}
		break;
		case PLAYER_UPDATE_PACKET:
		{
			
			/* Well i also modify values here that is present on the main thread but there is no lock guard here. 
			   The reason for that is no objects are added/removed, this is just pure information so if there is a problem 
			   there client that is supposed to be updated isn't then it won't be a problem. It will resolve itself the next couple of updates.
			   
			   This problem will ONLY occur whenever someone disconnected, so this thread tries to update that player on the main thread because that client is still 
			   present here but not on the main thread or vice versa. Over the next couple of updates/frames this problem is solved by itself. 
			   Not sure if you can really call it a problem. */

			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			PlayerPacket pData;
			pData.Serialize(false, bsIn);

			for (size_t i = 0; i < m_connectedPlayers.size(); i++)
			{
			
				if (m_connectedPlayers[i].guid == pData.guid)
				{
					m_connectedPlayers[i] = pData;
				
					if (m_networkPlayers.m_players[i].data.guid == pData.guid)
					{
						m_networkPlayers.m_players[i].data = pData;
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
			/* You get this if you are the one that started the server, essentially this means that the
			   server confirmed that you are the admin/serve creator. */
			m_serverOwner = true;
			logTrace("[CLIENT-ADMIN] Press E to start game!");
		}
		break;
		case SERVER_CURRENT_STATE:
		{
			/* Whenever the server or more specifically the admin/server creator decides to change the state of the server
			   (for example changing the state from "Waiting for other players" to "Starting the actual game") this package is received
			   so every client is aware of the server state change */
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			ServerStateChange stateChange;
			stateChange.Serialize(false, bsIn);

			if (stateChange.currentState == NetGlobals::SERVER_STATE::GAME_IS_STARTING) {
				for(size_t i = 0; i < 10; i++)
					logTrace("[GAME SERVER]******** GAME IS STARTING ********");
			}else if (stateChange.currentState == NetGlobals::SERVER_STATE::GAME_IN_SESSION) {
				for (size_t i = 0; i < 10; i++)
					logTrace("[GAME SERVER]******** GAME HAS STARTED ********");
			}
		}
		break;

		case SPELL_CREATED:
		{
			/* Whenever a client (that is not you) cast a spell you receive this with all the necessary information 
			   and the same logic about the threads that is applied whenever we create/delete/update a player is also present here. */
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			SpellPacket spellPacket;
			spellPacket.Serialize(false, bsIn);
			NetworkSpells::SpellEntity se;

			se.spellData = spellPacket;
			se.flag = NetGlobals::THREAD_FLAG::ADD;
			se.gameobject = nullptr;
			
			{
				std::lock_guard<std::mutex> lockGuard(m_networkSpells.m_mutex);
				m_networkSpells.m_entities.emplace_back(se);
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
				SpellPacket& activeSpell = m_activeSpells[i];

				/* If the spell packet matches the creator ID and spell ID */
				if (activeSpell.CreatorGUID == spellPacket.CreatorGUID &&
					activeSpell.SpellID == spellPacket.SpellID)
				{
					/* Update the internal information about the spell */
					activeSpell.Position = spellPacket.Position;
					activeSpell.Rotation = spellPacket.Rotation;
					
					/* Get the entity on the main thread */
					NetworkSpells::SpellEntity& spellEntity = m_networkSpells.m_entities[i];

					/* If it also matches then update the values of that entity. That entity is the same one that
					   will later on get renderer by the main thread */
					if (spellEntity.spellData.CreatorGUID == spellPacket.CreatorGUID &&
						spellEntity.spellData.SpellID == spellPacket.SpellID)
					{
						spellEntity.spellData.Position = spellPacket.Position;
						spellEntity.spellData.Rotation = spellPacket.Rotation;
					}
					else {
						/* Just as the "PLAYER_UPDATE" this will resolve itself a couple of frames later */
						logWarning("[CLIENT] Client skipped a update on a spell due to sync problems. (Should resolve itself with time)");
					}

					break;
				}
			}

		}

		break;

		case SPELL_DESTROY:
		{
			/* Destroying a spell that was created by another client both internally (in this class "m_activeSpells") 
			   and on the main thread, and that is why a lock guard is used here. */
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			SpellPacket spellPacket;
			spellPacket.Serialize(false, bsIn);
			bsIn.SetReadOffset(0);
			
			// The scope
			{
				std::lock_guard<std::mutex> lockGuard(m_networkSpells.m_mutex);
				NetworkSpells::SpellEntity* ne = findSpellEntityInNetworkSpells(spellPacket);
				if (ne != nullptr) {
					ne->flag = NetGlobals::THREAD_FLAG::REMOVE;
				}
			}

			removeActiveSpell(spellPacket);
		
		}
		break;

		case SPELL_PLAYER_HIT:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			PlayerPacket playerPacket;
			playerPacket.Serialize(false, bsIn);

			m_myPlayerDataPacket.health = playerPacket.health;

			logTrace("[CLIENT] My health is {0}", m_myPlayerDataPacket.health);
		}
		break;

		case GAME_START_COUNTDOWN:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			CountdownPacket countdownPacket;
			countdownPacket.Serialize(false, bsIn);
			logTrace("[GAME SERVER] Starts game in {0}...", countdownPacket.timeLeft / 1000);
		}
		break;

		case RESPAWN_TIME:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			CountdownPacket countdownPacket;
			countdownPacket.Serialize(false, bsIn);
			logTrace("[GAME SERVER] Respawn in {0}...", countdownPacket.timeLeft / 1000);
		}
		break;

		case RESPAWN_PLAYER:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			PlayerPacket playerPacket;
			playerPacket.Serialize(false, bsIn);

			m_myPlayerDataPacket.health = playerPacket.health;

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
/* Update the local information about the player (my player).
   This will get sent to the server whenever the processing thread
   wakes up from its beauty sleep. */
void Client::updatePlayerData(Player* player)
{
	if (!m_initialized || !m_isConnectedToAnServer) return;

	m_myPlayerDataPacket.position = player->getPlayerPos();
	m_myPlayerDataPacket.rotation = glm::vec3(
		player->getCamera()->getViewMat()[1][0] - glm::radians(90.0f),
		player->getCamera()->getViewMat()[1][1] - glm::radians(90.0f),
		player->getCamera()->getViewMat()[1][2] - glm::radians(90.0f)
	);
}

/* You created a spell locally and wants to tell the server and all the other clients that.
   You put it in a queue that will get emptied and sent to the server whenever the processing thread 
   wakes up from its beauty sleep. */
void Client::createSpellOnNetwork(const Spell& spell)
{
	if (!m_initialized || !m_isConnectedToAnServer) return;

	SpellPacket spellPacket;
	spellPacket.packetType = SPELL_CREATED;
	spellPacket.CreatorGUID = m_clientPeer->GetMyGUID();
	spellPacket.Position = spell.getTransform().position;
	spellPacket.Direction = spell.getDirection();
	spellPacket.SpellID = spell.getUniqueID();
	spellPacket.Rotation = glm::vec3(0.0f);
	spellPacket.Scale = spell.getTransform().scale;
	spellPacket.SpellType = (SPELL_TYPE)spell.getType();

	m_removeOrAddSpellQueue.emplace_back(spellPacket);
}


void Client::updateSpellOnNetwork(const Spell& spell)
{
	if (!m_initialized || !m_isConnectedToAnServer) return;

	SpellPacket spellPacket;
	spellPacket.packetType = SPELL_UPDATE;
	spellPacket.CreatorGUID = m_clientPeer->GetMyGUID();
	spellPacket.Position = spell.getTransform().position;
	spellPacket.SpellID = spell.getUniqueID();
	spellPacket.Direction = spell.getDirection();
	spellPacket.Rotation = glm::vec3(0.0f);
	spellPacket.Scale = spell.getTransform().scale;
	spellPacket.SpellType = (SPELL_TYPE)spell.getType(); 

	m_updateSpellQueue.emplace_back(spellPacket);
}

void Client::destroySpellOnNetwork(const Spell& spell)
{
	if (!m_initialized || !m_isConnectedToAnServer) return;

	SpellPacket spellPacket;
	spellPacket.packetType = SPELL_DESTROY;
	spellPacket.CreatorGUID = m_clientPeer->GetMyGUID();
	spellPacket.Position = spell.getTransform().position;
	spellPacket.SpellID = spell.getUniqueID();
	spellPacket.Direction = spell.getDirection();
	spellPacket.Rotation = glm::vec3(0.0f);
	spellPacket.Scale = spell.getTransform().scale;
	spellPacket.SpellType = (SPELL_TYPE)spell.getType();

	m_removeOrAddSpellQueue.emplace_back(spellPacket);
}

void Client::sendHitRequest(Spell& spell, NetworkPlayers::PlayerEntity& playerThatWasHit)
{
	if (!m_initialized || !m_isConnectedToAnServer) return;

	HitPacket hitPacket;
	hitPacket.SpellID = spell.getUniqueID();
	hitPacket.CreatorGUID = m_clientPeer->GetMyGUID();
	hitPacket.playerHitGUID = playerThatWasHit.data.guid.rakNetGuid;
	hitPacket.Position = spell.getTransform().position;
	hitPacket.Rotation = spell.getTransform().rotation;
	hitPacket.Scale = spell.getTransform().scale;
	hitPacket.SpellDirection = spell.getDirection();
	hitPacket.damage = spell.getSpellBase()->m_damage;
	
	m_spellsHitQueue.emplace_back(hitPacket);
}

void Client::updateNetworkEntities(const float& dt)
{
	if (m_initialized && m_isConnectedToAnServer) {
		m_networkPlayers.update(dt);
		m_networkSpells.update(dt);
	}

}

void Client::sendStartRequestToServer()
{
	if (m_serverOwner) {
		RakNet::BitStream stream;
		stream.Write((RakNet::MessageID)SERVER_CHANGE_STATE);
		ServerStateChange stateChange;
		stateChange.currentState = NetGlobals::SERVER_STATE::GAME_IS_STARTING;
		stateChange.Serialize(true, stream);
		m_clientPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
	}
}

void Client::updateDataOnServer()
{
	// Player data sent to server
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)PLAYER_UPDATE_PACKET);
	m_myPlayerDataPacket.Serialize(true, bsOut);
	m_clientPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);

	//// Empty out the spell queue
	//for (size_t i = 0; i < m_spellQueue.size(); i++) {
	//
	//	RakNet::BitStream bsOut;
	//	bsOut.Write(m_spellQueue[i].packetType);
	//	m_spellQueue[i].Serialize(true, bsOut);
	//	m_clientPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
	//}

	// Update all spells first
	for (size_t i = 0; i < m_updateSpellQueue.size(); i++) {
	
		RakNet::BitStream bsOut;
		bsOut.Write(m_updateSpellQueue[i].packetType);
		m_updateSpellQueue[i].Serialize(true, bsOut);
		m_clientPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
	}

	// Check collision with them
	for (size_t i = 0; i < m_spellsHitQueue.size(); i++) {

		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)SPELL_PLAYER_HIT);
		m_spellsHitQueue[i].Serialize(true, bsOut);
		m_clientPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
	}

	// remove or add new spells on the network
	for (size_t i = 0; i < m_removeOrAddSpellQueue.size(); i++) {

		RakNet::BitStream bsOut;
		bsOut.Write(m_removeOrAddSpellQueue[i].packetType);
		m_removeOrAddSpellQueue[i].Serialize(true, bsOut);
		m_clientPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
	}

	// Empty all the queues
	m_updateSpellQueue.clear();
	m_spellsHitQueue.clear();
	m_removeOrAddSpellQueue.clear();


	//m_spellQueue.clear();
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
	return m_networkPlayers;
}

NetworkSpells& Client::getNetworkSpellsREF()
{
	return m_networkSpells;
}

const PlayerPacket& Client::getMyData() const
{
	return m_myPlayerDataPacket;
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

void Client::setUsername(const std::string& userName)
{
	if (userName.size() > 16) {
		std::memcpy(m_userName, userName.c_str(), 16);
	}
	std::memcpy(m_myPlayerDataPacket.userName, userName.c_str(), userName.size());

	//m_myPlayerDataPacket.userName = m_userName;

}

const bool Client::doneRefreshingServerList() const
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
	for (size_t i = 0; i < m_networkSpells.m_entities.size(); i++) {
		
		SpellPacket& sp = m_networkSpells.m_entities[i].spellData;
	
		if (sp.CreatorGUID == packet.CreatorGUID && sp.SpellID == packet.SpellID) {
			return &m_networkSpells.m_entities[i];
		}
	}

	return nullptr;
}

NetworkPlayers::PlayerEntity* Client::findPlayerEntityInNetworkPlayers(const RakNet::AddressOrGUID& guid)
{
	for (size_t i = 0; i < m_networkPlayers.m_players.size(); i++) {

		PlayerPacket& p = m_networkPlayers.m_players[i].data;

		if (p.guid == guid) {
			return &m_networkPlayers.m_players[i];
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
