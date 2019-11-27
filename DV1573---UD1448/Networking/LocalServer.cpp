#include <Pch/Pch.h>
#include "LocalServer.h"


LocalServer::LocalServer()
{
	m_adminID = RakNet::UNASSIGNED_RAKNET_GUID;
	m_countdown = NetGlobals::WarmupCountdownTimeMS;
	m_roundTimer = NetGlobals::GameRoundTimeMS;

	m_timedCountdownTimer.setTotalExecutionTime(NetGlobals::WarmupCountdownTimeMS);
	m_timedCountdownTimer.setExecutionInterval(500);
	m_timedCountdownTimer.registerCallback(std::bind(&LocalServer::countdownExecutionLogic, this));

	m_timedRunTimer.setTotalExecutionTime(NetGlobals::GameRoundTimeMS);
	m_timedRunTimer.setExecutionInterval(500);
	m_timedRunTimer.registerCallback(std::bind(&LocalServer::roundTimeExecutionLogic, this));

	m_timedGameInEndStateTimer.setTotalExecutionTime(NetGlobals::InGameEndStateTimeMS);
	m_timedGameInEndStateTimer.setExecutionInterval(500);
	m_timedGameInEndStateTimer.registerCallback(std::bind(&LocalServer::endGameTimeExecutionLogic, this));

	m_timedPickupSpawner.setTotalExecutionTime(NetGlobals::GameRoundTimeMS);
	m_timedPickupSpawner.setExecutionInterval(NetGlobals::PickupSpawnIntervalMS);
	m_timedPickupSpawner.registerCallback(std::bind(&LocalServer::spawnPickup, this));

	m_updateClientsWithServertimeTimer.setInfinityExecutionTime(true);
	m_updateClientsWithServertimeTimer.setExecutionInterval(NetGlobals::UpdateClientsWithServerTimeIntervalMS);
	m_updateClientsWithServertimeTimer.registerCallback(std::bind(&LocalServer::m_updateClientsWithServertime, this));
	m_updateClientsWithServertimeTimer.start();

	m_pickupID = 0;
}

LocalServer::~LocalServer()
{
}

LocalServer* LocalServer::getInstance()
{
	static LocalServer s;
	return &s;
}

void LocalServer::startup(const std::string& serverName)
{
	if (!m_initialized) {
		m_serverPeer = RakNet::RakPeerInterface::GetInstance();
		auto startResult = m_serverPeer->Startup(NetGlobals::MaximumConnections, &RakNet::SocketDescriptor(NetGlobals::ServerPort, 0), 1);
		assert((startResult == RakNet::RAKNET_STARTED, "Server could not be started!"));

		m_serverPeer->SetMaximumIncomingConnections(NetGlobals::MaximumIncomingConnections);

		memcpy(&m_serverInfo.serverName, serverName.c_str(), serverName.length());
		m_serverInfo.currentState = NetGlobals::SERVER_STATE::WaitingForPlayers;
		m_serverInfo.maxPlayers = NetGlobals::MaximumPlayingPlayersOnServer;
		m_serverInfo.connectedPlayers = 0;
		m_connectedPlayers.reserve(NetGlobals::MaximumConnections);

		m_serverPeer->SetOfflinePingResponse((const char*)& m_serverInfo, sizeof(ServerInfo));

		if (m_processThread.joinable()) {
			m_processThread.join();
		}

		m_processThread = std::thread(&LocalServer::ThreadedUpdate, this);
		m_initialized = true;
		logTrace("[Server] Tickrate: {0}", NetGlobals::TickRate);
		logTrace("[Server] Thread sleep time {0}", NetGlobals::NetThreadSleepTime);
		m_serverPeer->SetTimeoutTime(NetGlobals::PlayerTimeoutTimeMS, RakNet::UNASSIGNED_SYSTEM_ADDRESS);

		m_adminID = RakNet::UNASSIGNED_RAKNET_GUID;
		createPickupSpawnLocations();
		createPlayerSpawnLocations();
	}
}

void LocalServer::destroy()
{
	if (m_initialized) {
		if (m_serverPeer != nullptr) {

			// In it's own scope	
			{
				std::lock_guard<std::mutex> lockGuard(m_cleanupMutex);
				m_shutdownServer = true;
			}

			logTrace("Waiting for server thread to finish...");
			if(m_processThread.joinable())
				m_processThread.join();
			logTrace("Server thread shutdown");

			m_connectedPlayers.clear();
			m_respawnList.clear();
			m_activeSpells.clear();
			m_pickupSpawnLocations.clear();
			m_activePickups.clear();
			m_queuedPickups.clear();
			m_playerSpawnLocations.clear();
			m_pickupID = 0;
		}
		m_initialized = false;
		m_adminID = RakNet::UNASSIGNED_RAKNET_GUID;
		resetServerData();
		RakNet::RakPeerInterface::DestroyInstance(m_serverPeer);
	}
}

void LocalServer::ThreadedUpdate()
{
	bool serverRunning = true;
	m_shutdownServer = false;
	uint32_t currentTimeMS = 0;
	uint32_t lastTimeMS = 0;
	uint32_t timeDiff = 0;

	unsigned int _time = unsigned int(time(NULL));
	srand(_time);

	while (serverRunning) {
		
		lastTimeMS = currentTimeMS;
		currentTimeMS = RakNet::GetTimeMS();
		timeDiff = (currentTimeMS - lastTimeMS);
		
		m_updateClientsWithServertimeTimer.update(static_cast<float>(timeDiff));

		if (m_serverInfo.currentState == NetGlobals::SERVER_STATE::GameIsStarting) {
			handleCountdown(timeDiff);
		}

		if (m_serverInfo.currentState == NetGlobals::SERVER_STATE::GameInSession) {
			handleRespawns(timeDiff);
			handleRoundTime(timeDiff);
			handlePickupTimer(timeDiff);
			checkCollisionBetweenPlayersAndPickups();
			updatePlayersWithDamageBuffs(timeDiff);
		}

		if (m_serverInfo.currentState == NetGlobals::SERVER_STATE::GameFinished) {
			handleEndGameStateTime(timeDiff);
		}

		processAndHandlePackets();

		// Scope
		{
			std::lock_guard<std::mutex> lockGuard(m_cleanupMutex);
			if (m_shutdownServer == true)
				serverRunning = false;
		}

		//m_timedUnusedObjectRemoval.update(static_cast<float>(timeDiff));

		for (size_t i = 0; i < m_connectedPlayers.size(); i++) {
			PlayerPacket& p = m_connectedPlayers[i];
			if (p.invulnerabilityTime > 0.0f) {
				p.invulnerabilityTime -= static_cast<float>(timeDiff) / 1000.0f;

				if (p.invulnerabilityTime <= 0.0f) {
					p.invulnerabilityTime = 0.0f;
				}
			}
			
		}

		RakSleep(NetGlobals::NetThreadSleepTime);
	}
}

void LocalServer::processAndHandlePackets()
{

	for (RakNet::Packet* packet = m_serverPeer->Receive(); packet; m_serverPeer->DeallocatePacket(packet), packet = m_serverPeer->Receive())
	{
		RakNet::BitStream bsIn(packet->data, packet->length, false);

		auto packetID = getPacketID(packet);

		switch (packetID)
		{
		case ID_UNCONNECTED_PING:
		{
			logTrace("[Server] Got a ping from {0}", packet->systemAddress.ToString());
		}
		break;

		case ID_NEW_INCOMING_CONNECTION:
		{
			logTrace("[Server] New connection from {0}\nAssigned GUID: {1}", packet->systemAddress.ToString(), packet->guid.ToString());
			
		}
		break;

		case SPECTATE_REQUEST:
		{
			logTrace("[Server] Got a spectator request.\n");

			RakNet::BitStream acceptStream;
			acceptStream.Write((RakNet::MessageID)PLAYER_ACCEPTED_TO_SERVER);
			m_serverPeer->Send(&acceptStream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, packet->guid, false);

			// Send info about all clients to the newly connected one
			RakNet::BitStream stream_otherPlayers;
			stream_otherPlayers.Write((RakNet::MessageID)INFO_ABOUT_OTHER_PLAYERS);
			stream_otherPlayers.Write(m_connectedPlayers.size());

			for (size_t i = 0; i < m_connectedPlayers.size(); i++)
			{
				m_connectedPlayers[i].Serialize(true, stream_otherPlayers);
			}
			m_serverPeer->Send(&stream_otherPlayers, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, packet->systemAddress, false);

			// Send info about all existing spells to the new player
			RakNet::BitStream stream_existingSpells;
			stream_existingSpells.Write((RakNet::MessageID)SPELL_ALL_EXISTING_SPELLS);

			size_t totalNumSpells = 0;
			for (auto const& item : m_activeSpells)
			{
				totalNumSpells += item.second.size();
			}
			stream_existingSpells.Write(totalNumSpells);

			for (auto& item : m_activeSpells)
			{
				auto& vec = item.second;

				for (size_t i = 0; i < vec.size(); i++) {
					vec[i].Serialize(true, stream_existingSpells);
				}
			}
			m_serverPeer->Send(&stream_existingSpells, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, packet->systemAddress, false);

			// Create the new player and assign it the correct GUID
			PlayerPacket player;
			player.guid = packet->guid;
			player.Spectator = true;
			player.timestamp = RakNet::GetTimeMS();
			
			// Lastly, add the new player to the local list of connected players
			m_connectedPlayers.emplace_back(player);

			// Send server state to the new player
			RakNet::BitStream stateStream;
			stateStream.Write((RakNet::MessageID)SERVER_CURRENT_STATE);
			ServerStateChange statePacket;
			statePacket.currentState = m_serverInfo.currentState;
			statePacket.Serialize(true, stateStream);
			m_serverPeer->Send(&stateStream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, packet->systemAddress, false);


			sendReadyInfoToPlayer(&player);

			break;
		}

		case PLAY_REQUEST:
		{
			logTrace("[Server] Got a play request.\n");

			int numPlayersPlaying = 0; // Not counting spectators

			for (int i = 0; i < m_connectedPlayers.size(); i++) {
				if (m_connectedPlayers[i].Spectator == false)
					numPlayersPlaying++;
			}

			if (numPlayersPlaying >= NetGlobals::MaximumPlayingPlayersOnServer || m_serverInfo.currentState != NetGlobals::SERVER_STATE::WaitingForPlayers)
			{
				logTrace("[Server] Denied play request.\n");
				m_serverPeer->CloseConnection(packet->guid, true);
				m_serverPeer->DeallocatePacket(packet);
				return;
			}

			logTrace("[Server] Actually creating a player");

			RakNet::BitStream acceptStream;
			acceptStream.Write((RakNet::MessageID)PLAYER_ACCEPTED_TO_SERVER);
			m_serverPeer->Send(&acceptStream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, packet->guid, false);


			// Is it safe enough to assume that the first player that joins is the admin?
			if (m_adminID == RakNet::UNASSIGNED_RAKNET_GUID) {
				m_adminID = packet->guid;
				RakNet::BitStream stream;
				stream.Write((RakNet::MessageID)ADMIN_PACKET);
				m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, m_adminID, false);

			}


			// Send info about all clients to the newly connected one
			RakNet::BitStream stream_otherPlayers;
			stream_otherPlayers.Write((RakNet::MessageID)INFO_ABOUT_OTHER_PLAYERS);
			
			stream_otherPlayers.Write(numPlayersPlaying);

			for (size_t i = 0; i < m_connectedPlayers.size(); i++)
			{
				if(!m_connectedPlayers[i].Spectator)
					m_connectedPlayers[i].Serialize(true, stream_otherPlayers);
			}
			m_serverPeer->Send(&stream_otherPlayers, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, packet->systemAddress, false);

			// Send info about all existing spells to the new player
			RakNet::BitStream stream_existingSpells;
			stream_existingSpells.Write((RakNet::MessageID)SPELL_ALL_EXISTING_SPELLS);

			size_t totalNumSpells = 0;
			for (auto const& item : m_activeSpells)
			{
				totalNumSpells += item.second.size();
			}
			stream_existingSpells.Write(totalNumSpells);

			for (auto& item : m_activeSpells)
			{
				auto& vec = item.second;

				for (size_t i = 0; i < vec.size(); i++) {
					vec[i].Serialize(true, stream_existingSpells);
				}
			}
			m_serverPeer->Send(&stream_existingSpells, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, packet->systemAddress, false);

			// Create the new player and assign it the correct GUID
			PlayerPacket player;
			player.guid = packet->guid;
			player.timestamp = RakNet::GetTimeMS();
			// Tell all other clients about the new client
			RakNet::BitStream stream_newPlayer;
			stream_newPlayer.Write((RakNet::MessageID)PLAYER_JOINED);
			player.Serialize(true, stream_newPlayer);

			sendStreamToAllClients(stream_newPlayer, RELIABLE_ORDERED_WITH_ACK_RECEIPT);

			// Lastly, add the new player to the local list of connected players
			m_connectedPlayers.emplace_back(player);

			// Update the general server information.
			m_serverInfo.connectedPlayers++;
			m_serverPeer->SetOfflinePingResponse((const char*)&m_serverInfo, sizeof(ServerInfo));

			// Send server state to the new player
			RakNet::BitStream stateStream;
			stateStream.Write((RakNet::MessageID)SERVER_CURRENT_STATE);
			ServerStateChange statePacket;
			statePacket.currentState = m_serverInfo.currentState;
			statePacket.Serialize(true, stateStream);
			m_serverPeer->Send(&stateStream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, packet->systemAddress, false);

			sendReadyInfoToPlayer(&player);

			break;
		}
		case ID_DISCONNECTION_NOTIFICATION:
		{
			PlayerPacket* pp = getSpecificPlayer(packet->guid);
			
			bool returnAfterHandlingLostPlayer = false;
			if (pp != nullptr && pp->Spectator) {
				returnAfterHandlingLostPlayer = true;
				logTrace("[Server] Player that left was a spectator");
			}

			bool playerWasAccepted = handleLostPlayer(*packet, bsIn);
			
			if (returnAfterHandlingLostPlayer)
				continue;

			auto item = m_activeSpells.find(packet->guid.g);

			if (item != m_activeSpells.end()) {
				//logTrace("Going to delete spell: {0}", spellPacket.toString());
				auto& spellVec = item._Ptr->_Myval.second;

				for (size_t i = 0; i < spellVec.size(); i++) {
					RakNet::BitStream stream;
					stream.Write((RakNet::MessageID)SPELL_DESTROY);
						
					spellVec[i].Serialize(true, stream);

					sendStreamToAllClients(stream, RELIABLE_ORDERED_WITH_ACK_RECEIPT);
				}

				m_activeSpells.erase(item);
			}

			if (playerWasAccepted) {
				unreadyEveryPlayer();
				logTrace("[Server] Player disconnected with {0}\nWith GUID: {1}", packet->systemAddress.ToString(), packet->guid.ToString());
				m_serverInfo.connectedPlayers--;
				m_serverPeer->SetOfflinePingResponse((const char*)&m_serverInfo, sizeof(ServerInfo));
			}
		}
		
		break;

		case ID_CONNECTION_LOST:
		{
			PlayerPacket* pp = getSpecificPlayer(packet->guid);

			bool returnAfterHandlingLostPlayer = false;
			if (pp != nullptr && pp->Spectator) {
				returnAfterHandlingLostPlayer = true;
			}

			bool playerWasAccepted = handleLostPlayer(*packet, bsIn);

			if (returnAfterHandlingLostPlayer)
				continue;

			if (playerWasAccepted) {
				unreadyEveryPlayer();
				logTrace("[Server] Lost connection with {0}\nWith GUID: {1}", packet->systemAddress.ToString(), packet->guid.ToString());
				m_serverInfo.connectedPlayers--;
				m_serverPeer->SetOfflinePingResponse((const char*)& m_serverInfo, sizeof(ServerInfo));
			}
		}
		break;

		case PLAYER_UPDATE_PACKET:
		{
			PlayerPacket playerPacket;
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			playerPacket.Serialize(false, bsIn);
			PlayerPacket* updatedPlayer = nullptr;
			// Update player on server
			bool foundPlayer = false;
			for (size_t i = 0; i < m_connectedPlayers.size() && foundPlayer == false; i++)
			{
				if (packet->guid == m_connectedPlayers[i].guid.rakNetGuid) {
					
					if (m_connectedPlayers[i].Spectator)
						continue;

					// HasBeenUpdatedOnce will go false always because the client won't update that variable & same goes for latest spawn pos & invulnerabilityTime
					bool hasBeenUpdatedOnce = m_connectedPlayers[i].hasBeenUpdatedOnce;
					glm::vec3 latestSpawnPos = m_connectedPlayers[i].latestSpawnPosition;
					float invulnerabilityTime = m_connectedPlayers[i].invulnerabilityTime;

					m_connectedPlayers[i] = playerPacket;
					
					m_connectedPlayers[i].hasBeenUpdatedOnce = hasBeenUpdatedOnce;
					m_connectedPlayers[i].latestSpawnPosition = latestSpawnPos;
					m_connectedPlayers[i].timestamp = RakNet::GetTimeMS();
					m_connectedPlayers[i].invulnerabilityTime = invulnerabilityTime;

					if (m_connectedPlayers[i].hasBeenUpdatedOnce == false) {
						m_connectedPlayers[i].hasBeenUpdatedOnce = true;
						memcpy(m_connectedPlayers[i].userName, playerPacket.userName, sizeof(playerPacket.userName));
						logTrace("[Server] Flagged player with guid {0} as updated atleast once", m_connectedPlayers[i].guid.ToString());
					}
					updatedPlayer = &m_connectedPlayers[i];

					if (updatedPlayer->position.y < -20) {
						hardRespawnPlayer(*updatedPlayer);
					}

					foundPlayer = true;
				}


				if (updatedPlayer != nullptr) {
					// Serialize it
					RakNet::BitStream stream;
					stream.Write((RakNet::MessageID)PLAYER_UPDATE_PACKET);
					updatedPlayer->Serialize(true, stream);

					// Send it to all clients except the sender
					for (size_t i = 0; i < m_connectedPlayers.size(); i++)
					{
						if (updatedPlayer->guid.rakNetGuid != m_connectedPlayers[i].guid.rakNetGuid) {
							m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_connectedPlayers[i].guid, false);
						}
					}
				}

			}
		}
		break;
		
		case SERVER_CHANGE_STATE:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			ServerStateChange statePacket;
			statePacket.Serialize(false, bsIn);
			
			if (m_serverInfo.currentState == NetGlobals::SERVER_STATE::WaitingForPlayers)
				stateChange(statePacket.currentState);
		}
		break;

		case SPELL_CREATED:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			SpellPacket spellPacket;
			spellPacket.Serialize(false, bsIn);
			bsIn.SetReadOffset(0);

			auto item = m_activeSpells.find(spellPacket.CreatorGUID.g);

			if (item == m_activeSpells.end()) {
				std::vector<SpellPacket> spellVec;
				spellVec.reserve(10);
				spellVec.emplace_back(spellPacket);
				m_activeSpells[spellPacket.CreatorGUID.g] = spellVec;
				//logTrace("[Server] Created a new list with spells for player {0}, spell ID {1}", spellPacket.CreatorGUID.ToString(), spellPacket.SpellID);
			}
			else {
				//logTrace("[Server] Added spell to spell list for player {0}, spell ID {1}", spellPacket.CreatorGUID.ToString(), spellPacket.SpellID);
				item._Ptr->_Myval.second.emplace_back(spellPacket);
			}
		
			for (size_t i = 0; i < m_connectedPlayers.size(); i++)
			{
				// Don't send it back to the sender
				if (packet->guid != m_connectedPlayers[i].guid.rakNetGuid) {
					m_serverPeer->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, m_connectedPlayers[i].guid, false);
				}

			}
		}
		break;

		case SPELL_UPDATE:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			SpellPacket spellPacket;
			spellPacket.Serialize(false, bsIn);
			bsIn.SetReadOffset(0);
			
			//logTrace("[Server] Update spell {0}", spellPacket.toString());
			auto item = m_activeSpells.find(spellPacket.CreatorGUID.g);

			if (item != m_activeSpells.end()) {
				auto& spellVec = item._Ptr->_Myval.second;

				for (size_t i = 0; i < spellVec.size(); i++) {
					if (spellVec[i].SpellID == spellPacket.SpellID) {
						spellVec[i] = spellPacket;

						spellVec[i].timestamp = RakNet::GetTimeMS();
						spellPacket.timestamp = spellVec[i].timestamp; // this will give the clients the server timestamp aswell
					}
				}

				for (size_t i = 0; i < m_connectedPlayers.size(); i++)
				{
					// Don't send it back to the sender
					if (packet->guid != m_connectedPlayers[i].guid.rakNetGuid) {
						RakNet::BitStream spellStream;
						spellStream.Write((RakNet::MessageID)SPELL_UPDATE);
						spellPacket.Serialize(true, spellStream);
						m_serverPeer->Send(&spellStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_connectedPlayers[i].guid, false);
					}

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

			auto item = m_activeSpells.find(spellPacket.CreatorGUID.g);

			if (item != m_activeSpells.end()) {
				//logTrace("Going to delete spell: {0}", spellPacket.toString());
				auto& spellVec = item._Ptr->_Myval.second;
				bool deleted = false;
				for (size_t i = 0; i < spellVec.size() && !deleted; i++) {
					
					if (spellVec[i].SpellID == spellPacket.SpellID) {
						//logTrace("[Server] Deleted a spell with spell ID: {0} from client {1}", spellVec[i].SpellID, spellVec[i].CreatorGUID.ToString());
						spellVec.erase(spellVec.begin() + i);
						deleted = true;
					}
				}

				if (spellVec.size() == 0)
				{
					//logTrace("[Server] Deleted the local spell container for client with ID {0}", spellPacket.CreatorGUID.ToString());
					m_activeSpells.erase(item);
				}

			}

			for (size_t i = 0; i < m_connectedPlayers.size(); i++)
			{
				// Don't send it back to the sender
				if (packet->guid != m_connectedPlayers[i].guid.rakNetGuid) {
					m_serverPeer->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, m_connectedPlayers[i].guid, false);
				}

			}

		}

		break;
		
		case SPELL_PLAYER_HIT:
		{
			if (m_serverInfo.currentState != NetGlobals::SERVER_STATE::GameInSession)
				continue;

			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			HitPacket hitPacket;
			hitPacket.Serialize(false, bsIn);
			bsIn.SetReadOffset(0);

			PlayerPacket* target = getSpecificPlayer(hitPacket.playerHitGUID);
			PlayerPacket* shooter = getSpecificPlayer(hitPacket.CreatorGUID);
			SpellPacket* spell = getSpecificSpell(hitPacket.CreatorGUID.g, hitPacket.SpellID);
			
			if (target == nullptr || spell == nullptr || shooter == nullptr || target->Spectator || shooter->Spectator)
				continue;
			
			if (target->health == 0.0f || target->invulnerabilityTime > 0.0f)
				continue;

			
			handleCollisionWithSpells(&hitPacket, spell, shooter, target);
			
			break;
		}

		case DESTRUCTION:
		{
			for (size_t i = 0; i < m_connectedPlayers.size(); i++)
			{
				// Don't send it back to the sender
				if (packet->guid != m_connectedPlayers[i].guid.rakNetGuid) {
					m_serverPeer->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, m_connectedPlayers[i].guid, false);
				}

			}
			break;
		}

		case READY_PACKET:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			ReadyPacket readyPacket;
			readyPacket.Serialize(false, bsIn);

			auto* player = getSpecificPlayer(readyPacket.guid);

			if (player != nullptr) {
				player->isReady = true;

				sendReadyInfoToAllPlayers();

				if (m_serverInfo.currentState == NetGlobals::SERVER_STATE::WaitingForPlayers) {

					if (getNumberOfReadyPlayers() >= getNumberOfPlayingPlayers()) {
						stateChange(NetGlobals::SERVER_STATE::GameIsStarting);
					}

				}


			}
			else
			{
				logError("[Server] Someone that is not registered on the server is apparently ready?");
			}

			break;
		}

		default:
		{
			
		}
			break;
		}

	}
}

void LocalServer::handleCollisionWithSpells(HitPacket* hitpacket, SpellPacket* spell, PlayerPacket* shooter, PlayerPacket* target)
{
	//create the axis and rotate them
	glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
	std::vector<glm::vec3> axis;

	glm::quat playerrot = glm::quat(target->rotation);
	xAxis = glm::rotate(playerrot, xAxis);
	yAxis = glm::rotate(playerrot, yAxis);
	zAxis = glm::rotate(playerrot, zAxis);

	axis.emplace_back(xAxis);
	axis.emplace_back(yAxis);
	axis.emplace_back(zAxis);

	if (specificSpellCollision(*spell, target, axis))
	{

		// It collided with a player, now check if the player is in deflect state. If the player is deflect state
		// then do some deflect logic otherwise just handle it as a normal collision.

		if (target->inDeflectState) {

			if (validDeflect(spell, target))
			{
				// Tell the target to create a new spell in his direction
				RakNet::BitStream spellCreationStream;
				spellCreationStream.Write((RakNet::MessageID)SPELL_GOT_DEFLECTED);
				spell->Serialize(true, spellCreationStream);
				m_serverPeer->Send(&spellCreationStream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, target->guid, false);

				return;
			}
		}

		//Update the shooter's hitmark
		RakNet::BitStream hitmarkStream;
		hitmarkStream.Write((RakNet::MessageID)HITMARK);
		shooter->Serialize(true, hitmarkStream);
		m_serverPeer->Send(&hitmarkStream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, shooter->guid, false);

		float damageMultiplier = 1.0f;

		if (shooter->hasDamageBuff)
			damageMultiplier = 4.0f;

		float totalDamage = hitpacket->damage * damageMultiplier;

		target->health -= static_cast<int>(totalDamage);

		target->lastHitByGuid = hitpacket->CreatorGUID;

		if (target->health <= 0) {
			removePlayerBuff(target);
			target->health = 0;
			Respawner respawner;
			respawner.currentTime = NetGlobals::TimeBeforeRespawnMS;
			respawner.player = target;
			m_respawnList.emplace_back(respawner);

			// Update the dead player score
			target->numberOfDeaths++;
			RakNet::BitStream hitPlayerStream;
			hitPlayerStream.Write((RakNet::MessageID)SCORE_UPDATE);
			target->Serialize(true, hitPlayerStream);
			m_serverPeer->Send(&hitPlayerStream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, target->guid, false);

			// Assign the dead player a spawn position
			target->latestSpawnPosition = getRandomSpawnLocation();
			logTrace("[Server] Assigned spawn position: {0}, {1}, {2}", target->latestSpawnPosition.x, target->latestSpawnPosition.y, target->latestSpawnPosition.z);

			// Update the shooters score
			if (shooter->guid.rakNetGuid != target->guid.rakNetGuid) {

				shooter->numberOfKills++;
				RakNet::BitStream shooterPacketStream;
				shooterPacketStream.Write((RakNet::MessageID)SCORE_UPDATE);
				shooter->Serialize(true, shooterPacketStream);
				m_serverPeer->Send(&shooterPacketStream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, shooter->guid, false);
			}

			// Send the kill feed information
			RakNet::BitStream killFeedStream;
			killFeedStream.Write((RakNet::MessageID)KILL_FEED);
			KillFeedPacket kFeed;
			kFeed.killerGuid = shooter->guid;
			kFeed.deadGuid = target->guid;
			kFeed.Serialize(true, killFeedStream);
			sendStreamToAllClients(killFeedStream, RELIABLE_ORDERED_WITH_ACK_RECEIPT);

		}

		// Send a hit packet to the target
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)SPELL_PLAYER_HIT);
		target->Serialize(true, bsOut);
		m_serverPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, hitpacket->playerHitGUID, false);

	}
}

bool LocalServer::validDeflect(SpellPacket* spell, PlayerPacket* target)
{
	float cosVal = glm::dot(glm::normalize(target->lookDirection), glm::normalize(spell->Direction));
	if (cosVal <= -0.3f)
		return true;

	return false;
}

bool LocalServer::specificSpellCollision(const SpellPacket& spellPacket, PlayerPacket* target, const std::vector<glm::vec3>& axis) {

	bool collision = false;
	//float sphereRadius = 2.0f * spellPacket.Scale.x * 2;
	float sphereRadius = 1.0f * spellPacket.Scale.x;
	
	//line is the walking we will do.
	float nrSubStep = 6.0f;
	glm::vec3 line = (spellPacket.Position - spellPacket.LastPosition) / nrSubStep;
	glm::vec3 interpolationPos = spellPacket.LastPosition;

	//walk from last pos to new pos with substeps
	for (size_t k = 0; k < nrSubStep; k++)
	{
		interpolationPos += line;

		float distx2 = OBBsqDist(interpolationPos, target, axis);

		if (distx2 <= sphereRadius * sphereRadius)
		{
			collision = true;
			k = static_cast<size_t>(nrSubStep);
		}
	}
	return collision;
}

float LocalServer::OBBsqDist(const glm::vec3& spellPosition, PlayerPacket* target, const std::vector<glm::vec3>& axis) {

	float dist = 0.0f;
	//glm::vec3 halfSize = glm::vec3(2.7f, 6.0f, 0.5f); // change this when real character is in
	glm::vec3 halfSize = target->meshHalfSize;

	//closest point on obb
	glm::vec3 boxPoint = target->position;
	boxPoint.y += halfSize.y;
	glm::vec3 ray = glm::vec3(spellPosition - boxPoint);

	for (int j = 0; j < 3; j++) {
		float distance = glm::dot(ray, axis.at(j));
		float distance2 = 0;

		if (distance > halfSize[j])
			distance2 = distance - halfSize[j];

		if (distance < -halfSize[j])
			distance2 = distance + halfSize[j];

		dist += distance2 * distance2;
	}
	return dist;
}

PlayerPacket* LocalServer::getSpecificPlayer(const RakNet::RakNetGUID& guid)
{
	for (size_t i = 0; i < m_connectedPlayers.size(); i++) {
		if (m_connectedPlayers[i].guid.rakNetGuid == guid)
		{
			return &m_connectedPlayers[i];
		}
	}

	return nullptr;
}

SpellPacket* LocalServer::getSpecificSpell(const uint64_t& creatorGUID, const uint64_t& spellID)
{
	auto item = m_activeSpells.find(creatorGUID);

	if (item != m_activeSpells.end()) {
		auto& spellVec = item._Ptr->_Myval.second;

		for (size_t i = 0; i < spellVec.size(); i++) {

			if (spellVec[i].SpellID == spellID) {
				return &spellVec[i];
			}
		}
	}

	return nullptr;
}

void LocalServer::checkCollisionBetweenPlayersAndPickups()
{
	for (size_t i = 0; i < m_activePickups.size(); i++) {
		auto& pickup = m_activePickups[i];

		for (size_t j = 0; j < m_connectedPlayers.size(); j++) {
			auto& player = m_connectedPlayers[j];
			
			if (!player.hasBeenUpdatedOnce || player.health <= 0 || player.Spectator) 
				continue;

			if (isCollidingWithPickup(player, pickup)) {
				// Give player buffs here
				if (pickup.type == PickupType::HealthPotion) {
					if(player.health < NetGlobals::PlayerMaxHealth)
						player.health = NetGlobals::PlayerMaxHealth;
					
					RakNet::BitStream stream;
					stream.Write((RakNet::MessageID)HEAL_BUFF);
					player.Serialize(true, stream);
					m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, player.guid, false);


				}
				else if (pickup.type == PickupType::DamageBuff) {
					player.hasDamageBuff = true;
					RakNet::BitStream stream;
					stream.Write((RakNet::MessageID)DAMAGE_BUFF_ACTIVE);
					player.health = NetGlobals::PlayerMaxHealth;
					player.Serialize(true, stream);
					m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, player.guid, false);

					BuffedPlayer buffedPlayer;
					buffedPlayer.currentTime = NetGlobals::DamageBuffActiveTimeMS;

					buffedPlayer.player = &player;
					m_buffedPlayers.emplace_back(buffedPlayer);

				}
				//----------------------

				destroyPickupOverNetwork(pickup);

				if (m_activePickups.size() == 3)
				{
					m_timedPickupSpawner.restartIntervalTimer();
				}

				m_activePickups.erase(m_activePickups.begin() + i);
				i--;

			}


		}


	}
}

bool LocalServer::isCollidingWithPickup(const PlayerPacket& player, const PickupPacket& pickup)
{
	float pickupRadius = 5.0f; // hard coded
	
	if (glm::distance(player.position, pickup.position) <= pickupRadius)
		return true;

	return false;
}

void LocalServer::updatePlayersWithDamageBuffs(const uint32_t& diff)
{
	for (size_t i = 0; i < m_buffedPlayers.size(); i++) {
		auto& buff = m_buffedPlayers[i];

		if (diff <= buff.currentTime)
			buff.currentTime -= diff;
		else
			buff.currentTime = 0;



		if (buff.currentTime <= 0) {
			buff.player->hasDamageBuff = false;
			RakNet::BitStream stream;
			stream.Write((RakNet::MessageID)DAMAGE_BUFF_INACTIVE);
			m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, buff.player->guid, false);
			m_buffedPlayers.erase(m_buffedPlayers.begin() + i);
			i--;
		}
	}
}

void LocalServer::handleRespawns(const uint32_t& diff)
{
	for (size_t i = 0; i < m_respawnList.size(); i++) {
		auto& rs = m_respawnList[i];

		if (diff < rs.currentTime) {
			rs.currentTime -= diff;
			RakNet::BitStream stream;
			stream.Write((RakNet::MessageID)RESPAWN_TIME);
			CountdownPacket countdownPacket;
			countdownPacket.timeLeft = rs.currentTime;
			countdownPacket.Serialize(true, stream);
			m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, rs.player->guid, false);
		}
		else {
			rs.currentTime = 0;
			rs.player->health = NetGlobals::PlayerMaxHealth;
			rs.player->invulnerabilityTime = NetGlobals::InvulnerabilityTime;
			RakNet::BitStream stream;
			stream.Write((RakNet::MessageID)RESPAWN_PLAYER_DURING_SESSION);
			rs.player->Serialize(true, stream);
			// Double send it...
			m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, rs.player->guid, false);
			m_respawnList.erase(m_respawnList.begin() + i);
			i--;
		}
	}
}

void LocalServer::hardRespawnPlayer(PlayerPacket& player)
{
	RakNet::BitStream stream;
	stream.Write((RakNet::MessageID)RESPAWN_PLAYER_NOT_IN_SESSION);
	m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, player.guid, false);
}

void LocalServer::respawnPlayers()
{
	for (size_t i = 0; i < m_connectedPlayers.size(); i++)
	{
		if(m_connectedPlayers[i].Spectator)
		{
			continue;
		}
		// Two cases:
		/*
			1. Player is dead then respawn him at the regular spawnpoint
			2. Player is alive then just give the player full health
		*/

		// The player is dead
		if (m_connectedPlayers[i].health <= 0)
		{
			m_connectedPlayers[i].health = NetGlobals::PlayerMaxHealth;
			m_connectedPlayers[i].invulnerabilityTime = NetGlobals::InvulnerabilityTime;
			RakNet::BitStream stream;
			stream.Write((RakNet::MessageID)RESPAWN_PLAYER_NOT_IN_SESSION);
			m_connectedPlayers[i].Serialize(true, stream);
			m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, m_connectedPlayers[i].guid, false);
		}
		// The player is not dead, just need full health
		else
		{
			m_connectedPlayers[i].health = NetGlobals::PlayerMaxHealth;
			m_connectedPlayers[i].invulnerabilityTime = NetGlobals::InvulnerabilityTime;
			RakNet::BitStream stream;
			stream.Write((RakNet::MessageID)GIVE_PLAYER_FULL_HEALTH);
			m_connectedPlayers[i].Serialize(true, stream);
			m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, m_connectedPlayers[i].guid, false);
		}

	}
	m_respawnList.clear();
}

void LocalServer::resetPlayerBuffs()
{
	for (size_t i = 0; i < m_buffedPlayers.size(); i++) {
		auto& buff = m_buffedPlayers[i];
		buff.player->hasDamageBuff = false;
		RakNet::BitStream stream;
		stream.Write((RakNet::MessageID)DAMAGE_BUFF_INACTIVE);
		m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, buff.player->guid, false);
	}
	m_buffedPlayers.clear();
}

void LocalServer::removePlayerBuff(const PlayerPacket* player)
{
	for (size_t i = 0; i < m_buffedPlayers.size(); i++) {
		auto& buff = m_buffedPlayers[i];

		if (buff.player->guid == player->guid) {
			buff.player->hasDamageBuff = false;
			RakNet::BitStream stream;
			stream.Write((RakNet::MessageID)DAMAGE_BUFF_INACTIVE);
			m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, buff.player->guid, false);
			m_buffedPlayers.erase(m_buffedPlayers.begin() + i);
			i--;
			
		}

	}
}

void LocalServer::resetScores()
{
	for (size_t i = 0; i < m_connectedPlayers.size(); i++)
	{
		m_connectedPlayers[i].numberOfDeaths = 0;
		m_connectedPlayers[i].numberOfKills = 0;

		RakNet::BitStream stream;
		stream.Write((RakNet::MessageID)SCORE_UPDATE);
		m_connectedPlayers[i].Serialize(true, stream);
		m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, m_connectedPlayers[i].guid, false);
	}
}

void LocalServer::handleCountdown(const uint32_t& diff)
{
	m_timedCountdownTimer.update(static_cast<float>(diff));

	if (m_timedCountdownTimer.isDone()) {
		stateChange(NetGlobals::SERVER_STATE::GameInSession);
	}

}

void LocalServer::countdownExecutionLogic()
{
	RakNet::BitStream stream;
	stream.Write((RakNet::MessageID)GAME_START_COUNTDOWN);
	CountdownPacket countdownPacket;
	countdownPacket.timeLeft = static_cast<uint32_t>(m_timedCountdownTimer.getTimeLeft());
	countdownPacket.Serialize(true, stream);
	sendStreamToAllClients(stream);
}

void LocalServer::handleRoundTime(const uint32_t& diff)
{
	m_timedRunTimer.update(static_cast<float>(diff));

	if (m_timedRunTimer.isDone()) {
		stateChange(NetGlobals::SERVER_STATE::GameFinished);
	}
}

void LocalServer::roundTimeExecutionLogic()
{
	RakNet::BitStream stream;
	stream.Write((RakNet::MessageID)GAME_ROUND_TIMER);
	RoundTimePacket roundTimePacket;
	roundTimePacket.minutes = (static_cast<uint32_t>(m_timedRunTimer.getTimeLeft()) / 1000) / 60;
	roundTimePacket.seconds = (static_cast<uint32_t>(m_timedRunTimer.getTimeLeft()) / 1000) % 60;
	roundTimePacket.Serialize(true, stream);
	sendStreamToAllClients(stream);
}

void LocalServer::handleEndGameStateTime(const uint32_t& diff)
{
	m_timedGameInEndStateTimer.update(static_cast<float>(diff));

	if (m_timedGameInEndStateTimer.isDone()) {
		stateChange(NetGlobals::SERVER_STATE::WaitingForPlayers);
	}
}

void LocalServer::endGameTimeExecutionLogic()
{
	RakNet::BitStream stream;
	stream.Write((RakNet::MessageID)GAME_ROUND_TIMER);
	RoundTimePacket roundTimePacket;
	roundTimePacket.minutes = (static_cast<uint32_t>(m_timedGameInEndStateTimer.getTimeLeft()) / 1000) / 60;
	roundTimePacket.seconds = (static_cast<uint32_t>(m_timedGameInEndStateTimer.getTimeLeft()) / 1000) % 60;
	roundTimePacket.Serialize(true, stream);
	sendStreamToAllClients(stream);
}

void LocalServer::handlePickupTimer(const uint32_t& diff)
{
	m_timedPickupSpawner.update(static_cast<float>(diff));

	if (m_timedPickupSpawner.getTimeLeftOnInterval() <= NetGlobals::PickupNotificationBeforeSpawnMS && !m_pickupNotified) {
		m_pickupNotified = true;
		notifyPickup();
	}

	if (m_timedPickupSpawner.isDone()) {
		m_queuedPickups.clear();

	}
}

void LocalServer::spawnPickup()
{
	if (gameAlmostFinished()) {
		logTrace("[Server] Game is almost finished so no pickups will be spawned!");
		return;
	}


	for (size_t i = 0; i < m_queuedPickups.size(); i++) {
		RakNet::BitStream stream;
		stream.Write((RakNet::MessageID)PICKUP_CREATED);
		m_queuedPickups[i].Serialize(true, stream);
		sendStreamToAllClients(stream, RELIABLE_ORDERED_WITH_ACK_RECEIPT);
		m_activePickups.emplace_back(m_queuedPickups[i]);
	}

	m_queuedPickups.clear();
	m_pickupNotified = false;
}

void LocalServer::notifyPickup()
{
	if (gameAlmostFinished()) {
		logTrace("[Server] Game is almost finished so no pickups will be spawned!");
		return;
	}
	PickupSpawnLocation* spawnLocation = getRandomPickupSpawnLocation();

	if (spawnLocation == nullptr) {
		logTrace("[Server] There is no free spawn locations for pickups, so no pickup will be spawned!");
		return;
	}

	PickupPacket pickupPacket;
	copyCharArrayOver(pickupPacket.locationName, spawnLocation->name);
	pickupPacket.uniqueID = m_pickupID++;
	pickupPacket.type = getRandomPickupType(); // this should be randomized too
	pickupPacket.position = spawnLocation->position;
	
	m_queuedPickups.emplace_back(pickupPacket);

	RakNet::BitStream stream;
	stream.Write((RakNet::MessageID)PICKUP_NOTIFICATION);
	pickupPacket.Serialize(true, stream);
	sendStreamToAllClients(stream, RELIABLE_ORDERED_WITH_ACK_RECEIPT); 
}

bool LocalServer::gameAlmostFinished()
{
	return m_timedRunTimer.getTimeLeft() <= 15.0f * 1000.0f;
}

void LocalServer::m_updateClientsWithServertime()
{
	RakNet::BitStream stream;
	stream.Write((RakNet::MessageID)SERVER_TIME);
	ServerTimePacket time;
	time.serverTimestamp = RakNet::GetTimeMS();
	time.Serialize(true, stream);
	sendStreamToAllClients(stream);
}

void LocalServer::resetServerData()
{
	m_serverInfo.connectedPlayers = 0;
	m_serverInfo.currentState = NetGlobals::SERVER_STATE::WaitingForPlayers;
	m_serverInfo.maxPlayers = NetGlobals::MaximumConnections;
	char t[16] = { ' ' };
	memcpy(m_serverInfo.serverName, t, sizeof(m_serverInfo.serverName));
}

void LocalServer::createPickupSpawnLocations()
{
	
	PickupSpawnLocation spawn_one;
	copyStringToCharArray(spawn_one.name, "Top");
	spawn_one.position = glm::vec3(0.0f, 13.45f, 0.0f);
	m_pickupSpawnLocations.emplace_back(spawn_one);

	PickupSpawnLocation spawn_two;
	copyStringToCharArray(spawn_two.name, "Graveyard");
	spawn_two.position = glm::vec3(78.64f, 2.3f, 16.23f);
	m_pickupSpawnLocations.emplace_back(spawn_two);

	PickupSpawnLocation spawn_three;
	copyStringToCharArray(spawn_three.name, "Maze");
	spawn_three.position = glm::vec3(-94.42f, 2.6f, 2.08f);
	m_pickupSpawnLocations.emplace_back(spawn_three);

	PickupSpawnLocation spawn_four;
	copyStringToCharArray(spawn_four.name, "Tunnels");
	spawn_four.position = glm::vec3(0.282f, 2.4f, 53.94f);
	m_pickupSpawnLocations.emplace_back(spawn_four);

}

void LocalServer::createPlayerSpawnLocations()
{
	m_playerSpawnLocations.emplace_back(1.0f, 36.0f, -2.73f);
	m_playerSpawnLocations.emplace_back(59.0f, 3.0f, 6.0f);
	m_playerSpawnLocations.emplace_back(36.0f, 7.5f, -51.43f);

	m_playerSpawnLocations.emplace_back(-37.55f, 7.5f, -51.3f);
	m_playerSpawnLocations.emplace_back(-33.6f, 2.0f, 60.0f);
	m_playerSpawnLocations.emplace_back(-76.0f, 2.0f, 24.0f);
}

void LocalServer::destroyPickupOverNetwork(PickupPacket& pickupPacket)
{
	RakNet::BitStream stream;
	stream.Write((RakNet::MessageID)PICKUP_REMOVED);
	pickupPacket.Serialize(true, stream);
	sendStreamToAllClients(stream, RELIABLE_ORDERED_WITH_ACK_RECEIPT);

}

void LocalServer::copyStringToCharArray(char Dest[16], std::string Src)
{
	memset(Dest, ' ', 16);

	size_t size = Src.size();
	if (size > 14)
	{
		size = 14;
	}

	memcpy(Dest, Src.c_str(), size * sizeof(char));
	Dest[size + 1] = '\0';
}

void LocalServer::copyCharArrayOver(char Dest[16], char Src[16])
{
	memset(Dest, ' ', 16);
	memcpy(Dest, Src, sizeof(char) * 16);
}

void LocalServer::destroyAllPickups()
{
	for (size_t i = 0; i < m_activePickups.size(); i++) {
		destroyPickupOverNetwork(m_activePickups[i]);
	}
	m_activePickups.clear();
}

void LocalServer::sendReadyInfoToPlayer(const PlayerPacket* player)
{
	ReadyPlayersCount rdyPlayersPacket;
	rdyPlayersPacket.numberOfReadyPlayers = getNumberOfReadyPlayers();


	RakNet::BitStream playerCountStream;
	playerCountStream.Write((RakNet::MessageID)NUMBER_OF_READY_PLAYERS);
	rdyPlayersPacket.Serialize(true, playerCountStream);
	m_serverPeer->Send(&playerCountStream, IMMEDIATE_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, player->guid, false);
	
}

void LocalServer::sendReadyInfoToAllPlayers()
{
	ReadyPlayersCount rdyPlayersPacket;
	rdyPlayersPacket.numberOfReadyPlayers = getNumberOfReadyPlayers();

	RakNet::BitStream playerCountStream;
	playerCountStream.Write((RakNet::MessageID)NUMBER_OF_READY_PLAYERS);
	rdyPlayersPacket.Serialize(true, playerCountStream);
	sendStreamToAllClients(playerCountStream, RELIABLE_ORDERED_WITH_ACK_RECEIPT);
}

void LocalServer::unreadyEveryPlayer()
{
	for(size_t i = 0; i < m_connectedPlayers.size(); i++){
		PlayerPacket* player = &m_connectedPlayers[i];
		player->isReady = false;
	}

	RakNet::BitStream stream;
	stream.Write((RakNet::MessageID)UNREADY_PACKET_FROM_SERVER);
	sendStreamToAllClients(stream, RELIABLE_ORDERED_WITH_ACK_RECEIPT);

}

PickupType LocalServer::getRandomPickupType()
{

	/*size_t luckyNumber = Randomizer::single(size_t(0), size_t(1));

	if (luckyNumber == 0) {
		return PickupType::HealthPotion;
	}
	else if (luckyNumber == 1) {
		return PickupType::DamageBuff;
	}*/

	return PickupType::HealthPotion;
}

LocalServer::PickupSpawnLocation* LocalServer::getRandomPickupSpawnLocation()
{
	std::vector<PickupSpawnLocation*> availableLocations;
	availableLocations.reserve(m_pickupSpawnLocations.size()); // arbitrary number

	for (size_t i = 0; i < m_pickupSpawnLocations.size(); i++) {

		bool isFree = true;
		for (size_t j = 0; j < m_activePickups.size() && isFree == true; j++) {
			if (m_activePickups[j].position == m_pickupSpawnLocations[i].position) {
				isFree = false;
			}
		}

		if (isFree == true) {
			availableLocations.emplace_back(&m_pickupSpawnLocations[i]);
		}
	}

	if (availableLocations.size() > 0) {
		size_t luckyNumber = Randomizer::single(size_t(0), availableLocations.size() - 1);
		return availableLocations[luckyNumber];
	}


	return nullptr;
}

const glm::vec3& LocalServer::getRandomSpawnLocation()
{
	std::vector<glm::vec3*> availableSpawnLocations;

	for (size_t j = 0; j < m_playerSpawnLocations.size(); j++) {

		bool occupied = false;

		for (size_t i = 0; i < m_respawnList.size() && occupied; i++) {
			const Respawner& rs = m_respawnList[i];
			if (rs.player->latestSpawnPosition == m_playerSpawnLocations[j]) {
				occupied = true;
			}
		}

		if (occupied == false) {
			availableSpawnLocations.emplace_back(&m_playerSpawnLocations[j]);
		}

	}

	
	if(availableSpawnLocations.size() == 0)
		return m_playerSpawnLocations[0];


	size_t luckyNumber = Randomizer::single(size_t(0), availableSpawnLocations.size() - 1);
	return *availableSpawnLocations[luckyNumber];

}

const bool& LocalServer::isInitialized() const
{
	return m_initialized;
}

const ServerInfo& LocalServer::getMySeverInfo() const
{
	return m_serverInfo;
}

int LocalServer::getNumberOfReadyPlayers() const
{
	int count = 0;
	for (size_t i = 0; i < m_connectedPlayers.size(); i++) {
		if (m_connectedPlayers[i].isReady)
			count++;
	}

	return count;
}

int LocalServer::getNumberOfPlayingPlayers() const
{
	int count = 0;
	for (size_t i = 0; i < m_connectedPlayers.size(); i++) {
		if (!m_connectedPlayers[i].Spectator)
			count++;
	}

	return count;
}

unsigned char LocalServer::getPacketID(RakNet::Packet* p)
{
	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	else
		return (unsigned char)p->data[0];
}

bool LocalServer::handleLostPlayer(const RakNet::Packet& packet, const RakNet::BitStream& bsIn)
{
	RakNet::AddressOrGUID guid = packet.guid;

	RakNet::BitStream stream_disconnectedPlayer;
	stream_disconnectedPlayer.Write((RakNet::MessageID)PLAYER_DISCONNECTED);
	stream_disconnectedPlayer.Write(guid);

	size_t indexOfDisconnectedPlayer = -1;
	for (size_t i = 0; i < m_connectedPlayers.size(); i++)
	{
		// Save the index of the disconnected player and send a "Player_disconnected" packet to the others
		if (m_connectedPlayers[i].guid == guid)
		{
			indexOfDisconnectedPlayer = i;
		}
	}

	// Remove the disconnected player from the local list of clients
	if (indexOfDisconnectedPlayer != -1) {
		bool wasSpectator = m_connectedPlayers[indexOfDisconnectedPlayer].Spectator;
		m_connectedPlayers.erase(m_connectedPlayers.begin() + indexOfDisconnectedPlayer);
		
		// Only send them if the player that left/disconnected was accepted otherwise
		// the clients have no track record of them so they don't need to know anything.
		// Same if the disconnected player was a spectator.
		if (wasSpectator)
			return true;

		for (size_t i = 0; i < m_connectedPlayers.size(); i++) {
			logTrace("[Server] Sending disconnection packets to guid: {0}", m_connectedPlayers[i].guid.ToString());
			m_serverPeer->Send(&stream_disconnectedPlayer, IMMEDIATE_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, m_connectedPlayers[i].guid, false);
		}
		return true;
	}

	return false;


}

void LocalServer::stateChange(NetGlobals::SERVER_STATE newState)
{
	if (newState == m_serverInfo.currentState) return;

	m_serverInfo.currentState = newState;
	m_serverPeer->SetOfflinePingResponse((const char*)& m_serverInfo, sizeof(ServerInfo));
	
	RakNet::BitStream stream;
	stream.Write((RakNet::MessageID)SERVER_CURRENT_STATE);
	ServerStateChange statePacket;
	statePacket.currentState = newState;
	statePacket.Serialize(true, stream);
	sendStreamToAllClients(stream, RELIABLE_ORDERED_WITH_ACK_RECEIPT);

	if (newState == NetGlobals::SERVER_STATE::WaitingForPlayers) {
		unreadyEveryPlayer();
		respawnPlayers();
		resetScores();
		destroyAllPickups();
		resetPlayerBuffs();
		m_queuedPickups.clear();
		logTrace("[Server] Warmup!");
	}

	if (newState == NetGlobals::SERVER_STATE::GameIsStarting) {
		logTrace("[Server] Admin requested to start the game!");
		m_timedCountdownTimer.restart();
		m_timedCountdownTimer.start();
		m_timedCountdownTimer.forceExecute();
	}

	if (newState == NetGlobals::SERVER_STATE::GameInSession) {
		logTrace("[Server] Game has officially started!");
		m_timedRunTimer.restart();
		m_timedRunTimer.start();
		m_timedRunTimer.forceExecute();

		m_timedPickupSpawner.restart();
		m_timedPickupSpawner.start();
	}

	if (newState == NetGlobals::SERVER_STATE::GameFinished) {
		logTrace("[Server] Game is over!");
		respawnPlayers();
		resetPlayerBuffs();
		destroyAllPickups();
		unreadyEveryPlayer();
		m_queuedPickups.clear();
		m_timedGameInEndStateTimer.restart();
		m_timedGameInEndStateTimer.start();
		m_timedGameInEndStateTimer.forceExecute();
	}

}

void LocalServer::sendStreamToAllClients(RakNet::BitStream& stream, PacketReliability flag)
{
	for (size_t i = 0; i < m_connectedPlayers.size(); i++)
	{
		m_serverPeer->Send(&stream, HIGH_PRIORITY, flag, 0, m_connectedPlayers[i].guid, false);
	}
}
