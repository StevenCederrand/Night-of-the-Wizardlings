#include <Pch/Pch.h>
#include "LocalServer.h"


LocalServer::LocalServer()
{
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
		m_serverInfo.maxPlayers = NetGlobals::MaximumConnections;
		m_serverInfo.connectedPlayers = 0;
		m_connectedPlayers.reserve(NetGlobals::MaximumConnections);

		m_serverPeer->SetOfflinePingResponse((const char*)& m_serverInfo, sizeof(ServerInfo));
		m_processThread = std::thread(&LocalServer::threadedProcess, this);
		m_initialized = true;
	}
}

void LocalServer::destroy()
{
	if (m_initialized) {
		if (m_serverPeer != nullptr) {
			logTrace("Waiting for server thread to finish...");
			m_shutdownServer = true;
			m_processThread.join();
			logTrace("Server thread shutdown");
		}
		m_initialized = false;
		RakNet::RakPeerInterface::DestroyInstance(m_serverPeer);
	}
}

void LocalServer::threadedProcess()
{
	while (!m_shutdownServer) {

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
				PlayerData player;
				player.guid = packet->guid;

				// Tell all other clients about the new client
				RakNet::BitStream stream_newPlayer;
				stream_newPlayer.Write((RakNet::MessageID)PLAYER_JOINED);
				player.Serialize(true, stream_newPlayer);
				
				for (size_t i = 0; i < m_connectedPlayers.size(); i++)
				{
					m_serverPeer->Send(&stream_newPlayer, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_connectedPlayers[i].guid, false);
				}

				// Lastly, add the new player to the local list of connected players
				m_connectedPlayers.emplace_back(player);

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
				m_serverInfo.connectedPlayers--;
				m_serverPeer->SetOfflinePingResponse((const char*)& m_serverInfo, sizeof(ServerInfo));
			}
			break;

			case PLAYER_DATA:
			{
				for (size_t i = 0; i < m_connectedPlayers.size(); i++)
				{	
					// Don't send it back to the sender
					if(packet->guid != m_connectedPlayers[i].guid.rakNetGuid)
						m_serverPeer->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_connectedPlayers[i].guid, false);
				}
			}
			break;
			}

		}

		RakSleep(NetGlobals::networkRefreshRate);
	}
}

const bool& LocalServer::isInitialized() const
{
	return m_initialized;
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
