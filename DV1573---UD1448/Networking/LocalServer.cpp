#include <Pch/Pch.h>
#include "LocalServer.h"


LocalServer::LocalServer()
{
	m_adminID = RakNet::UNASSIGNED_RAKNET_GUID;
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
		m_serverInfo.currentState = NetGlobals::ServerState::WaitingForPlayers;
		m_serverInfo.maxPlayers = NetGlobals::MaximumConnections;
		m_serverInfo.connectedPlayers = 0;
		m_connectedPlayers.reserve(NetGlobals::MaximumConnections);

		m_serverPeer->SetOfflinePingResponse((const char*)& m_serverInfo, sizeof(ServerInfo));
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
		}
		m_initialized = false;
		RakNet::RakPeerInterface::DestroyInstance(m_serverPeer);
	}
}

void LocalServer::ThreadedUpdate()
{
	bool serverRunning = true;

	while (serverRunning) {

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

			// Is it safe enough to assume that the first player that joins is the admin?
			if (m_adminID == RakNet::UNASSIGNED_RAKNET_GUID) {
				m_adminID = packet->guid;
				RakNet::BitStream stream;
				stream.Write((RakNet::MessageID)ADMIN_PACKET);
				m_serverPeer->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_adminID, false);

			}

			if (m_connectedPlayers.size() >= NetGlobals::MaximumConnections) logError("[SERVER]  Trying to add more clients than allowed!");

			RakNet::BitStream stream_otherPlayers;

			// Send info about all clients to the newly connected one
			stream_otherPlayers.Write((RakNet::MessageID)INFO_ABOUT_OTHER_PLAYERS);
			stream_otherPlayers.Write(m_connectedPlayers.size());

			for (size_t i = 0; i < m_connectedPlayers.size(); i++)
			{
				m_connectedPlayers[i].Serialize(true, stream_otherPlayers);
			}
			m_serverPeer->Send(&stream_otherPlayers, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

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
			logTrace("[SERVER] Player disconnected with {0}\nWith GUID: {1}", packet->systemAddress.ToString(), packet->guid.ToString());
			handleLostPlayer(*packet, bsIn);
			m_serverInfo.connectedPlayers--;
			m_serverPeer->SetOfflinePingResponse((const char*)& m_serverInfo, sizeof(ServerInfo));

		}
		break;

		case ID_CONNECTION_LOST:
		{
			logTrace("[SERVER] Lost connection with {0}\nWith GUID: {1}", packet->systemAddress.ToString(), packet->guid.ToString());
			handleLostPlayer(*packet, bsIn);
			
			// Update the general server information.
			m_serverInfo.connectedPlayers--;
			m_serverPeer->SetOfflinePingResponse((const char*)& m_serverInfo, sizeof(ServerInfo));
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
					bsIn.SetReadOffset(0);
				}

			}
		}
		break;
		
		case SERVER_CHANGE_STATE:
		{
			stateChange(NetGlobals::ServerState::GameStarted);
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

		}

		break;

		default:
			break;
		}

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

void LocalServer::handleLostPlayer(const RakNet::Packet& packet, const RakNet::BitStream& bsIn)
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
		else
		{
			logTrace("Sending disconnection packets to guid: {0}", m_connectedPlayers[i].guid.ToString());
			m_serverPeer->Send(&stream_disconnectedPlayer, IMMEDIATE_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, m_connectedPlayers[i].guid, false);
		}
	}

	// Remove the disconnected player from the local list of clients
	m_connectedPlayers.erase(m_connectedPlayers.begin() + indexOfDisconnectedPlayer);


}

void LocalServer::stateChange(NetGlobals::ServerState newState)
{
	if (newState == m_serverInfo.currentState) return;

	if(newState == NetGlobals::ServerState::GameStarted)
		logTrace("[SERVER] Admin requested to start the game!");

	m_serverInfo.currentState = newState;
	m_serverPeer->SetOfflinePingResponse((const char*)& m_serverInfo, sizeof(ServerInfo));
	
	RakNet::BitStream stream;
	stream.Write((RakNet::MessageID)SERVER_CURRENT_STATE);
	ServerStateChange statePacket;
	statePacket.currentState = NetGlobals::ServerState::GameStarted;
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
