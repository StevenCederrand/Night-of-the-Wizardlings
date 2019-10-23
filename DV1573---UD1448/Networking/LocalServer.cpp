#include <Pch/Pch.h>
#include "LocalServer.h"


LocalServer::LocalServer()
{
	m_adminID = RakNet::UNASSIGNED_RAKNET_GUID;
	m_countdown = NetGlobals::serverCountdownTimeMS;
	m_roundTimer = NetGlobals::roundTime;
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
		m_serverInfo.currentState = NetGlobals::SERVER_STATE::WAITING_FOR_PLAYERS;
		m_serverInfo.maxPlayers = NetGlobals::MaximumConnections;
		m_serverInfo.connectedPlayers = 0;
		m_connectedPlayers.reserve(NetGlobals::MaximumConnections);

		m_serverPeer->SetOfflinePingResponse((const char*)& m_serverInfo, sizeof(ServerInfo));

		if (m_processThread.joinable()) {
			m_processThread.join();
		}

		m_processThread = std::thread(&LocalServer::ThreadedUpdate, this);
		m_initialized = true;
		logTrace("[SERVER] Tickrate: {0}", NetGlobals::tickRate);
		logTrace("[SERVER] Thread sleep time {0}", NetGlobals::threadSleepTime);
		m_serverPeer->SetTimeoutTime(NetGlobals::timeoutTimeMS, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
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
		}
		m_initialized = false;
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
	while (serverRunning) {
		
		lastTimeMS = currentTimeMS;
		currentTimeMS = RakNet::GetTimeMS();
		timeDiff = (currentTimeMS - lastTimeMS);
		
		if (m_serverInfo.currentState == NetGlobals::SERVER_STATE::GAME_IS_STARTING) {
			handleCountdown(timeDiff);
		}

		if (m_serverInfo.currentState == NetGlobals::SERVER_STATE::GAME_IN_SESSION) {
			handleRespawns(timeDiff);
			handleRoundTime(timeDiff);
		}

		processAndHandlePackets();

		// Scope
		{
			std::lock_guard<std::mutex> lockGuard(m_cleanupMutex);
			if (m_shutdownServer == true)
				serverRunning = false;
		}

		RakSleep(NetGlobals::threadSleepTime);
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
			logTrace("[SERVER] Got a ping from {0}", packet->systemAddress.ToString());
		}
		break;

		case ID_NEW_INCOMING_CONNECTION:
		{
			logTrace("[SERVER] New connection from {0}\nAssigned GUID: {1}", packet->systemAddress.ToString(), packet->guid.ToString());
			
			if (m_connectedPlayers.size() >= NetGlobals::MaximumConnections || m_serverInfo.currentState != NetGlobals::SERVER_STATE::WAITING_FOR_PLAYERS)
			{
				m_serverPeer->CloseConnection(packet->guid, true);
				m_serverPeer->DeallocatePacket(packet);
				return;
			}

			logTrace("[SERVER] Actually creating a player");

			RakNet::BitStream acceptStream;
			acceptStream.Write((RakNet::MessageID)PLAYER_ACCEPTED_TO_SERVER);
			m_serverPeer->Send(&acceptStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->guid, false);


			// Is it safe enough to assume that the first player that joins is the admin?
			if (m_adminID == RakNet::UNASSIGNED_RAKNET_GUID) {
				m_adminID = packet->guid;
				RakNet::BitStream stream;
				stream.Write((RakNet::MessageID)ADMIN_PACKET);
				m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_adminID, false);

			}

			RakNet::BitStream stream_otherPlayers;

			// Send info about all clients to the newly connected one
			stream_otherPlayers.Write((RakNet::MessageID)INFO_ABOUT_OTHER_PLAYERS);
			stream_otherPlayers.Write(m_connectedPlayers.size());

			for (size_t i = 0; i < m_connectedPlayers.size(); i++)
			{
				m_connectedPlayers[i].Serialize(true, stream_otherPlayers);
			}
			m_serverPeer->Send(&stream_otherPlayers, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

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
			m_serverPeer->Send(&stream_existingSpells, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

			// Create the new player and assign it the correct GUID
			PlayerPacket player;
			player.guid = packet->guid;

			// Tell all other clients about the new client
			RakNet::BitStream stream_newPlayer;
			stream_newPlayer.Write((RakNet::MessageID)PLAYER_JOINED);
			player.Serialize(true, stream_newPlayer);

			sendStreamToAllClients(stream_newPlayer);

			// Lastly, add the new player to the local list of connected players
			m_connectedPlayers.emplace_back(player);

			// Update the general server information.
			m_serverInfo.connectedPlayers++;
			m_serverPeer->SetOfflinePingResponse((const char*)& m_serverInfo, sizeof(ServerInfo));

		}
		break;

		case ID_DISCONNECTION_NOTIFICATION:
		{
			bool playerWasAccepted = handleLostPlayer(*packet, bsIn);
			if (playerWasAccepted) {
				auto item = m_activeSpells.find(packet->guid.g);

				if (item != m_activeSpells.end()) {
					//logTrace("Going to delete spell: {0}", spellPacket.toString());
					auto& spellVec = item._Ptr->_Myval.second;

					for (size_t i = 0; i < spellVec.size(); i++) {
						RakNet::BitStream stream;
						stream.Write((RakNet::MessageID)SPELL_DESTROY);
						
						spellVec[i].Serialize(true, stream);

						sendStreamToAllClients(stream);
					}

					m_activeSpells.erase(item);
				}

				logTrace("[SERVER] Player disconnected with {0}\nWith GUID: {1}", packet->systemAddress.ToString(), packet->guid.ToString());
				m_serverInfo.connectedPlayers--;
				m_serverPeer->SetOfflinePingResponse((const char*)& m_serverInfo, sizeof(ServerInfo));
			}
		}
		break;

		case ID_CONNECTION_LOST:
		{
			bool playerWasAccepted = handleLostPlayer(*packet, bsIn);
			if (playerWasAccepted) {
				logTrace("[SERVER] Lost connection with {0}\nWith GUID: {1}", packet->systemAddress.ToString(), packet->guid.ToString());
				m_serverInfo.connectedPlayers--;
				m_serverPeer->SetOfflinePingResponse((const char*)& m_serverInfo, sizeof(ServerInfo));
			}
		}
		break;

		case PLAYER_UPDATE_PACKET:
		{
			for (size_t i = 0; i < m_connectedPlayers.size(); i++)
			{
				// Don't send it back to the sender
				if (packet->guid != m_connectedPlayers[i].guid.rakNetGuid) {
					m_serverPeer->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_connectedPlayers[i].guid, false);
				}
				// If it's the sender then update the information about that client on the server
				else if(packet->guid == m_connectedPlayers[i].guid.rakNetGuid){
					PlayerPacket playerPacket;
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					playerPacket.Serialize(false, bsIn);
					m_connectedPlayers[i] = playerPacket;
				/*	m_connectedPlayers[i].guid = playerPacket.guid;
					m_connectedPlayers[i].position = playerPacket.position;
					m_connectedPlayers[i].rotation = playerPacket.rotation;*/
					
					bsIn.SetReadOffset(0);
				}

			}
		}
		break;
		
		case SERVER_CHANGE_STATE:
		{
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			ServerStateChange statePacket;
			statePacket.Serialize(false, bsIn);

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
				logTrace("[SERVER] Created a new list with spells for player {0}, spell ID {1}", spellPacket.CreatorGUID.ToString(), spellPacket.SpellID);
			}
			else {
				logTrace("[SERVER] Added spell to spell list for player {0}, spell ID {1}", spellPacket.CreatorGUID.ToString(), spellPacket.SpellID);
				item._Ptr->_Myval.second.emplace_back(spellPacket);
			}
		
			for (size_t i = 0; i < m_connectedPlayers.size(); i++)
			{
				// Don't send it back to the sender
				if (packet->guid != m_connectedPlayers[i].guid.rakNetGuid) {
					m_serverPeer->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_connectedPlayers[i].guid, false);
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

			//logTrace("[SERVER] Update spell {0}", spellPacket.toString());
			auto item = m_activeSpells.find(spellPacket.CreatorGUID.g);

			if (item != m_activeSpells.end()) {
				auto& spellVec = item._Ptr->_Myval.second;

				for (size_t i = 0; i < spellVec.size(); i++) {
					if (spellVec[i].SpellID == spellPacket.SpellID) {
						spellVec[i].Position = spellPacket.Position;
						spellVec[i].Rotation = spellPacket.Rotation;
						spellVec[i].Direction = spellPacket.Direction;
					}
				}

				for (size_t i = 0; i < m_connectedPlayers.size(); i++)
				{
					// Don't send it back to the sender
					if (packet->guid != m_connectedPlayers[i].guid.rakNetGuid) {
						m_serverPeer->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_connectedPlayers[i].guid, false);
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
						logTrace("[SERVER] Deleted a spell with spell ID: {0} from client {1}", spellVec[i].SpellID, spellVec[i].CreatorGUID.ToString());
						spellVec.erase(spellVec.begin() + i);
						deleted = true;
					}
				}

				if (spellVec.size() == 0)
				{
					logTrace("[SERVER] Deleted the local spell container for client with ID {0}", spellPacket.CreatorGUID.ToString());
					m_activeSpells.erase(item);
				}

			}

			for (size_t i = 0; i < m_connectedPlayers.size(); i++)
			{
				// Don't send it back to the sender
				if (packet->guid != m_connectedPlayers[i].guid.rakNetGuid) {
					m_serverPeer->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_connectedPlayers[i].guid, false);
				}

			}

		}

		break;
			  
		case SPELL_PLAYER_HIT:
		{
			if (m_serverInfo.currentState != NetGlobals::SERVER_STATE::GAME_IN_SESSION)
				return;

			logTrace("PLAYER HIT PACKAGE");

			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			HitPacket hitPacket;
			hitPacket.Serialize(false, bsIn);
			bsIn.SetReadOffset(0);

			PlayerPacket* pp = getSpecificPlayer(hitPacket.playerHitGUID);
			PlayerPacket* shooter = getSpecificPlayer(hitPacket.CreatorGUID);
			SpellPacket* sp = getSpecificSpell(hitPacket.CreatorGUID.g, hitPacket.SpellID);
			if (pp == nullptr || sp == nullptr || shooter == nullptr) {
				logTrace("[SERVER] Player or spell was null");
				return;
			}

			if (pp->health == 0.0f || shooter->health == 0.0f)
				return;

			//create the axis and rotate them
			glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
			glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
			glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
			std::vector<glm::vec3> axis;

			glm::rotateX(xAxis, pp->rotation.x);
			glm::rotateY(xAxis, pp->rotation.y);
			glm::rotateZ(xAxis, pp->rotation.z);

			axis.emplace_back(xAxis);
			axis.emplace_back(yAxis);
			axis.emplace_back(zAxis);

		
			if (specificSpellCollision(*sp, pp->position, axis))
			{
				logTrace("[SERVER] sending hit package to client");
				pp->health -= static_cast<int>(hitPacket.damage);

				if (pp->health <= 0) {
					pp->health = 0;
					Respawner respawner;
					respawner.currentTime = NetGlobals::timeUntilRespawnMS;
					respawner.player = pp;
					m_respawnList.emplace_back(respawner);
				}

				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)SPELL_PLAYER_HIT);
				pp->Serialize(true, bsOut);
				m_serverPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, hitPacket.playerHitGUID, false);

			}
		}

		break;

		default:
		{
			logTrace("Unknown package");
		}
			break;
		}

	}
}

bool LocalServer::specificSpellCollision(const SpellPacket& spellPacket, const glm::vec3& playerPos, const std::vector<glm::vec3>& axis) {

	bool collision = false;
	float sphereRadius = 1.0f * spellPacket.Scale.x * 2;

	glm::vec3 closestPoint = OBBclosestPoint(spellPacket, axis, playerPos);
	glm::vec3 v = closestPoint - spellPacket.Position;

	if (glm::dot(v, v) <= sphereRadius * sphereRadius)
	{
		collision = true;
	}
	return collision;
}

glm::vec3 LocalServer::OBBclosestPoint(const SpellPacket& spellPacket, const std::vector<glm::vec3>& axis, const glm::vec3& playerPos) {

	float boxSize = 0.5f;
	//closest point on obb
	glm::vec3 boxPoint = playerPos;
	glm::vec3 ray = glm::vec3(spellPacket.Position - playerPos);

	for (int j = 0; j < 3; j++) {
		float distance = glm::dot(ray, axis.at(j));
		float distance2 = 0;

		if (distance > boxSize)
			distance2 = boxSize;

		if (distance < -boxSize)
			distance2 = -boxSize;


		boxPoint += distance2 * axis.at(j);
	}

	return boxPoint;
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
			rs.player->health = NetGlobals::maxPlayerHealth;

			RakNet::BitStream stream;
			stream.Write((RakNet::MessageID)RESPAWN_PLAYER);
			rs.player->Serialize(true, stream);
			m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, rs.player->guid, false);

			m_respawnList.erase(m_respawnList.begin() + i);
			i--;
		}
	}
}

void LocalServer::handleCountdown(const uint32_t& diff)
{
	if (diff <= m_countdown) {
		m_countdown -= diff;
		RakNet::BitStream stream;
		stream.Write((RakNet::MessageID)GAME_START_COUNTDOWN);
		CountdownPacket countdownPacket;
		countdownPacket.timeLeft = m_countdown;
		countdownPacket.Serialize(true, stream);
		sendStreamToAllClients(stream);

	}
	else {
		m_countdown = NetGlobals::serverCountdownTimeMS;
		stateChange(NetGlobals::SERVER_STATE::GAME_IN_SESSION);
	}
}

void LocalServer::handleRoundTime(const uint32_t& diff)
{
	static float t = 1000.f;

	if (diff <= m_roundTimer) {
		m_roundTimer -= diff;
		t -= diff;
		
		if (t <= 0.0f) {
			t = 1000.f;
			RakNet::BitStream stream;
			stream.Write((RakNet::MessageID)GAME_ROUND_TIMER);
			RoundTimePacket roundTimePacket;
			roundTimePacket.minutes = (m_roundTimer / 1000) / 60;
			roundTimePacket.seconds = (m_roundTimer / 1000) % 60;
			roundTimePacket.Serialize(true, stream);
			sendStreamToAllClients(stream);
		}
	}
	else {
		m_roundTimer = NetGlobals::roundTime;
		t = 1000.f;
		stateChange(NetGlobals::SERVER_STATE::WAITING_FOR_PLAYERS);
		// RESTART
	}
}

const bool& LocalServer::isInitialized() const
{
	return m_initialized;
}

const ServerInfo& LocalServer::getMySeverInfo() const
{
	return m_serverInfo;
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
		m_connectedPlayers.erase(m_connectedPlayers.begin() + indexOfDisconnectedPlayer);
		
		// Only send them if the player that left/disconnected was accepted otherwise
		// the clients have no track record of them so they don't need to know anything
		for (size_t i = 0; i < m_connectedPlayers.size(); i++) {
			logTrace("[SERVER] Sending disconnection packets to guid: {0}", m_connectedPlayers[i].guid.ToString());
			m_serverPeer->Send(&stream_disconnectedPlayer, IMMEDIATE_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, m_connectedPlayers[i].guid, false);
		}
		return true;
	}

	return false;


}

void LocalServer::stateChange(NetGlobals::SERVER_STATE newState)
{
	if (newState == m_serverInfo.currentState) return;

	if (newState == NetGlobals::SERVER_STATE::GAME_IS_STARTING)
		logTrace("[SERVER] Admin requested to start the game!");

	if (newState == NetGlobals::SERVER_STATE::GAME_IN_SESSION)
		logTrace("[SERVER] Game has officially started!");

	m_serverInfo.currentState = newState;
	m_serverPeer->SetOfflinePingResponse((const char*)& m_serverInfo, sizeof(ServerInfo));
	
	RakNet::BitStream stream;
	stream.Write((RakNet::MessageID)SERVER_CURRENT_STATE);
	ServerStateChange statePacket;
	statePacket.currentState = newState;
	statePacket.Serialize(true, stream);
	sendStreamToAllClients(stream);

}

void LocalServer::sendStreamToAllClients(RakNet::BitStream& stream)
{
	for (size_t i = 0; i < m_connectedPlayers.size(); i++)
	{
		m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_connectedPlayers[i].guid, false);
	}
}
