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
		m_sendUpdatePackages = false;

		m_networkPickup = new NetworkPickups();
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
	

		m_serverList.clear();
		m_connectedPlayers.clear();
		m_activeSpells.clear();
		m_spellsHitQueue.clear();
		m_updateSpellQueue.clear();
		m_removeOrAddSpellQueue.clear();
		m_networkPlayers.cleanUp();
		m_networkSpells.cleanUp();
		m_networkPickup->cleanUp();
		delete m_networkPickup;
		resetPlayerData();
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
	assert((status == true, "[Client] Client connecting to {0} failed!", server.serverName));

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
		logTrace("[Client] Sent a disconnect package to server :)");
		RakNet::BitStream stream;
		stream.Write((RakNet::MessageID)ID_DISCONNECTION_NOTIFICATION);
		m_clientPeer->Send(&stream, IMMEDIATE_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, m_serverAddress, false);
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
			logTrace("[Client] Connected to server but not sure if actually accepted to the server.\n");
		}
		break;

		case ID_CONNECTION_ATTEMPT_FAILED:
			logTrace("[Client] Connection failed, server might be full.\n");
			m_isConnectedToAnServer = false;
			m_failedToConnect = true;
			m_shutdownThread = true;
			break;

		case ID_ALREADY_CONNECTED:
			logTrace("[Client] You are already connected to the server\n");
			break;

		case ID_CONNECTION_BANNED:
			logTrace("[Client] You are banned for that server.\n");
			break;

		case ID_INVALID_PASSWORD:
			logTrace("[Client] Invalid server password.\n");
			break;

		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			m_shutdownThread = true;
			logTrace("[Client] Client Error: incompatible protocol version!\n");
			break;

		case ID_NO_FREE_INCOMING_CONNECTIONS:
			logTrace("[Client] Client Error: No free incoming connection slots!\n");
			m_failedToConnect = true;
			m_isConnectedToAnServer = false;
			m_shutdownThread = true;
			break;

		case ID_DISCONNECTION_NOTIFICATION:
			logTrace("[Client] Disconnected from server!\n");
			m_failedToConnect = true;
			m_isConnectedToAnServer = false;
			m_shutdownThread = true;
			break;

		case ID_CONNECTION_LOST:
			logTrace("[Client] Connection to the server is lost!\n");
			m_isConnectedToAnServer = false;
			m_shutdownThread = true;
			break;

		case PLAYER_ACCEPTED_TO_SERVER:
		{
			/* The server accepted you which means that the server had room for you & wasn't in session */
			logTrace("[Client] Connected and accepted by server! Welcome!.\n");
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
					std::lock_guard<std::mutex> lockGuard(m_networkPlayers.m_mutex);
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
			logTrace("[Client] Got packet with all the existing spells");
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
						logWarning("[Client] Client skipped a update on a client due to sync problems. (Should resolve itself with time)");
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
			logTrace("[Client-ADMIN] Press E to start game!");
		}
		break;
		case SERVER_CURRENT_STATE:
		{
			/* Whenever the server or more specifically the admin/server creator decides to change the state of the server
			   (for example changing the state from "Waiting for other players" to "Starting the actual game") this package is received
			   so every client is aware of the server state change */
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

			m_serverState.Serialize(false, bsIn);
			if (m_serverState.currentState == NetGlobals::SERVER_STATE::WAITING_FOR_PLAYERS) {
				logTrace("[Client]******** WARMUP ********");
			}
			else if (m_serverState.currentState == NetGlobals::SERVER_STATE::GAME_IS_STARTING) {
				logTrace("[Client]******** GAME IS STARTING ********");
			}
			else if (m_serverState.currentState == NetGlobals::SERVER_STATE::GAME_IN_SESSION) {
				logTrace("[Client]******** GAME HAS STARTED ********");
			}
			else if (m_serverState.currentState == NetGlobals::SERVER_STATE::GAME_END_STATE) {
				logTrace("[Client]******** GAME HAS ENDED ********");

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
						//logWarning("[Client] Client skipped a update on a spell due to sync problems. (Should resolve itself with time)");
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
			m_myPlayerDataPacket.lastHitByGuid = playerPacket.lastHitByGuid;
			m_myPlayerDataPacket.health = playerPacket.health;
			m_latestPlayerThatHitMe = findPlayerByGuid(playerPacket.lastHitByGuid);
			
			// Add this to the event list
			{
				std::lock_guard<std::mutex> lockGuard(m_playerEventMutex); // Thread safe
				m_playerEvents.push_back(PlayerEvents::TookDamage);

				if (m_myPlayerDataPacket.health <= 0) {
					m_playerEvents.push_back(PlayerEvents::Died);
					
				}
			}

		}
		break;

		case GAME_START_COUNTDOWN:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			m_countDownPacket.Serialize(false, bsIn);
			m_inGame = true;
		}
		break;

		case RESPAWN_TIME:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			m_respawnTime.Serialize(false, bsIn);
		}
		break;

		case GAME_ROUND_TIMER:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			m_roundTimePacket.Serialize(false, bsIn);
		}
		break;

		case RESPAWN_PLAYER:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			PlayerPacket playerPacket;
			playerPacket.Serialize(false, bsIn);
			logTrace("[Client] Spawn at: {0}, {1}, {2}", playerPacket.latestSpawnPosition.x, playerPacket.latestSpawnPosition.y, playerPacket.latestSpawnPosition.z);
			m_myPlayerDataPacket.latestSpawnPosition = playerPacket.latestSpawnPosition;
			m_myPlayerDataPacket.health = playerPacket.health;

			// Add this to the event list
			{
				std::lock_guard<std::mutex> lockGuard(m_playerEventMutex); // Thread safe
				m_playerEvents.push_back(PlayerEvents::Respawned);
			}


		}
		break;

		case SCORE_UPDATE:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			PlayerPacket playerPacket;
			playerPacket.Serialize(false, bsIn);
			m_myPlayerDataPacket.numberOfDeaths = playerPacket.numberOfDeaths;
			m_myPlayerDataPacket.numberOfKills = playerPacket.numberOfKills;
		}

		break;

		case SPELL_GOT_DEFLECTED:
		{
			
			if (m_spellHandler == nullptr) continue;

			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			SpellPacket spellPacket;
			spellPacket.Serialize(false, bsIn);
			bsIn.SetReadOffset(0);
			
			SpellHandler::deflectSpellData data;
			data.position = m_myPlayerDataPacket.position;
			data.direction = m_myPlayerDataPacket.lookDirection;
			data.type = spellPacket.SpellType;
			
			// scope
			{
				std::lock_guard<std::mutex> lockGuard(m_spellHandler->m_clientSyncMutex);
				m_spellHandler->m_deflectedSpells.emplace_back(data);
			}
		}
		break;

		case PICKUP_CREATED:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			PickupPacket pickupPacket;
			pickupPacket.Serialize(false, bsIn);
			NetworkPickups::PickupProp pp;

			pp.flag = NetGlobals::THREAD_FLAG::ADD;
			pp.packet = pickupPacket;
			pp.pickup = nullptr;
			
			{
				std::lock_guard<std::mutex> lockGuard(m_networkPickup->m_mutex);
				m_networkPickup->m_pickupProps.emplace_back(pp);
			}

		}
		break;

		case PICKUP_REMOVED: 
		{
			logTrace("Pickup Removed");
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			PickupPacket pickupPacket;
			pickupPacket.Serialize(false, bsIn);

			bool found = false;
			for (size_t i = 0; i < m_networkPickup->m_pickupProps.size() && !found; i++) {
				auto& prop = m_networkPickup->m_pickupProps[i];
				
				if (prop.packet.uniqueID == pickupPacket.uniqueID) {
					std::lock_guard<std::mutex> lockGuard(m_networkPickup->m_mutex);
					prop.flag = NetGlobals::THREAD_FLAG::REMOVE;
					found = true;
				}
			}
		}
		break;

		case PICKUP_NOTIFICATION: 
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			PickupPacket pickupPacket;
			pickupPacket.Serialize(false, bsIn);

			PickupNotificationText t;
			t.alphaColor = 1.0f;
			t.width = 0;
			t.scale = glm::vec3(0.60f);

			if (pickupPacket.type == PickupType::HealthPotion) {
				std::string type = "Health potion ";
				glm::vec3 color = glm::vec3(1.0f, 0.2f, 0.2f);
				t.width += Renderer::getInstance()->getTextWidth(type, t.scale);
				t.textParts.emplace_back(type, color);
			}
			else if (pickupPacket.type == PickupType::DamageBuff)
			{
				std::string type = "Damage potion ";
				glm::vec3 color = glm::vec3(1.0f, 0.5f, 0.0f);
				t.width += Renderer::getInstance()->getTextWidth(type, t.scale);
				t.textParts.emplace_back(type, color);
			}
		
			std::string text = "will spawn soon at " + std::string(pickupPacket.locationName) + "!";
			t.width += Renderer::getInstance()->getTextWidth(text, t.scale);
			glm::vec3 locColor = glm::vec3(1.0f, 1.0f, 1.0f);
			t.textParts.emplace_back(text, locColor);

			{
				renderPickupNotificationsMutexGuard();
				Renderer::getInstance()->addPickupNotificationText(t);
			}

		}
		break;

		case HEAL_BUFF:
		{
			
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			PlayerPacket pData;
			pData.Serialize(false, bsIn);

			// Add this to the event list
			{
				std::lock_guard<std::mutex> lockGuard(m_playerEventMutex); // Thread safe
				m_playerEvents.push_back(PlayerEvents::TookHeal);
			}

			m_myPlayerDataPacket.health = pData.health;
		}
		break;

		case DAMAGE_BUFF_ACTIVE:
		{
			// Add this to the event list
			{
				std::lock_guard<std::mutex> lockGuard(m_playerEventMutex); // Thread safe
				m_playerEvents.push_back(PlayerEvents::TookPowerup);
			}

			m_myPlayerDataPacket.hasDamageBuff = true;
		}
		break;

		case DAMAGE_BUFF_INACTIVE: 
		{	
			// Add this to the event list
			{
				std::lock_guard<std::mutex> lockGuard(m_playerEventMutex); // Thread safe
				m_playerEvents.push_back(PlayerEvents::PowerupRemoved);
			}

			m_myPlayerDataPacket.hasDamageBuff = false;
		}
		
		default:
		{
			
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
	m_myPlayerDataPacket.inDeflectState = player->isDeflecting();
	m_myPlayerDataPacket.lookDirection = player->getCamera()->getCamFace();
	m_myPlayerDataPacket.timestamp = RakNet::GetTimeMS();
	m_myPlayerDataPacket.rotation = glm::vec3(
		-glm::radians(player->getCamera()->getPitch()),
		-glm::radians(player->getCamera()->getYaw() - 90.0f),
		0.0f);

	if (m_sendUpdatePackages == false)
		m_sendUpdatePackages = true;
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
	spellPacket.timestamp = RakNet::GetTimeMS();
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
	spellPacket.timestamp = RakNet::GetTimeMS();
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
	spellPacket.timestamp = RakNet::GetTimeMS();
	spellPacket.Position = spell.getTransform().position;
	spellPacket.SpellID = spell.getUniqueID();
	spellPacket.Direction = spell.getDirection();
	spellPacket.Rotation = glm::vec3(0.0f);
	spellPacket.Scale = spell.getTransform().scale;
	spellPacket.SpellType = (SPELL_TYPE)spell.getType();

	m_removeOrAddSpellQueue.emplace_back(spellPacket);
}

void Client::requestToDestroyClientSpell(const SpellPacket& packet)
{
	m_removalOfClientSpellsQueue.emplace_back(packet);
}

void Client::sendHitRequest(Spell& spell, NetworkPlayers::PlayerEntity& playerThatWasHit)
{
	if (!m_initialized || !m_isConnectedToAnServer || m_serverState.currentState != NetGlobals::SERVER_STATE::GAME_IN_SESSION) return;

	HitPacket hitPacket;
	hitPacket.SpellID = spell.getUniqueID();
	hitPacket.CreatorGUID = m_clientPeer->GetMyGUID();
	hitPacket.playerHitGUID = playerThatWasHit.data.guid.rakNetGuid;
	hitPacket.Position = spell.getTransform().position;
	hitPacket.Rotation = spell.getTransform().rotation;
	hitPacket.Scale = spell.getTransform().scale;
	hitPacket.damage = spell.getDamage();
	hitPacket.SpellDirection = spell.getDirection();

	m_spellsHitQueue.emplace_back(hitPacket);
}

void Client::updateNetworkEntities(const float& dt)
{
	if (m_initialized && m_isConnectedToAnServer) {
		m_networkPlayers.update(dt);
		m_networkSpells.update(dt);
		m_networkPickup->update(dt);
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
	if (!m_sendUpdatePackages) {
		return;
	}
	// Scope
	{
		// Player data sent to server
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)PLAYER_UPDATE_PACKET);
		m_myPlayerDataPacket.Serialize(true, bsOut);
		m_clientPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
	}

	//// Update all spells first
	//for (size_t i = 0; i < m_removalOfClientSpellsQueue.size(); i++) {

	//	RakNet::BitStream bsOut;
	//	bsOut.Write((RakNet::MessageID)SPELL_REMOVAL_REQUEST);
	//	m_removalOfClientSpellsQueue[i].Serialize(true, bsOut);
	//	m_clientPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, m_serverAddress, false);
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
		m_clientPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, m_serverAddress, false);
	}

	// Empty all the queues
	m_updateSpellQueue.clear();
	m_spellsHitQueue.clear();
	m_removeOrAddSpellQueue.clear();
	m_removalOfClientSpellsQueue.clear();

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

const PlayerPacket* Client::getLatestPlayerThatHitMe() const
{
	return m_latestPlayerThatHitMe;
}

const ServerStateChange& Client::getServerState() const
{
	return m_serverState;
}

const CountdownPacket& Client::getCountdownPacket() const
{
	return m_countDownPacket;
}

const CountdownPacket& Client::getRespawnTime() const
{
	return m_respawnTime;
}

const RoundTimePacket& Client::getRoundTimePacket() const
{
	return m_roundTimePacket;
}

const PlayerEvents Client::readNextEvent()
{
	if (m_playerEvents.size() == 0)
		return PlayerEvents::None;

	// Save it
	PlayerEvents evnt = m_playerEvents[0];

	// Remove it so that the next time this function is called the next event will be shown
	{
		std::lock_guard<std::mutex> lockGuard(m_playerEventMutex); // Thread safe
		m_playerEvents.erase(m_playerEvents.begin());
	}
	// Return the event
	return evnt;
}

const std::vector<SpellPacket>& Client::getNetworkSpells()
{
	return m_activeSpells;
}

void Client::refreshServerList()
{
	logTrace("[Client] Fetching server list...");
	m_isRefreshingServerList = true;
	m_serverList.clear();
	findAllServerAddresses();
}

void Client::startSendingUpdatePackages()
{
	m_sendUpdatePackages = true;
}

void Client::assignSpellHandler(SpellHandler* spellHandler)
{
	m_spellHandler = spellHandler;
}

void Client::setUsername(const std::string& userName)
{
	if (userName.size() > 16) {
		std::memcpy(m_userName, userName.c_str(), 16);
	}
	std::memcpy(m_myPlayerDataPacket.userName, userName.c_str(), userName.size());
}

void Client::renderPickupNotificationsMutexGuard()
{
	std::lock_guard<std::mutex> lockGuard(m_renderPickupNotificationMutex);
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

const bool& Client::isServerOwner() const
{
	return m_serverOwner;
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

PlayerPacket* Client::findPlayerByGuid(const RakNet::AddressOrGUID& guid)
{
	for (size_t i = 0; i < m_connectedPlayers.size(); i++) {
		if (m_connectedPlayers[i].guid == guid) {
			return &m_connectedPlayers[i];
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

void Client::resetPlayerData()
{
	m_myPlayerDataPacket.guid = m_clientPeer->GetMyGUID();
	m_myPlayerDataPacket.health = NetGlobals::maxPlayerHealth;
	m_myPlayerDataPacket.inDeflectState = false;
	m_myPlayerDataPacket.numberOfDeaths = 0;
	m_myPlayerDataPacket.numberOfKills = 0;
	m_myPlayerDataPacket.hasBeenUpdatedOnce = false;
	char t[16] = { ' ' };
	memcpy(m_myPlayerDataPacket.userName, t, sizeof(m_myPlayerDataPacket.userName));
}
