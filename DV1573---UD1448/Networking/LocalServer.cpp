#include <Pch/Pch.h>
#include "LocalServer.h"


LocalServer::LocalServer()
{
}

LocalServer::~LocalServer()
{
	if (m_serverPeer != nullptr) {
		m_shutdownServer = true;
		m_processThread.join();
		logTrace("Server thread shutdown");
	}

	RakNet::RakPeerInterface::DestroyInstance(m_serverPeer);
	
}

void LocalServer::startup(const std::string& serverName)
{
	m_serverPeer = RakNet::RakPeerInterface::GetInstance();
	auto startResult = m_serverPeer->Startup(NetGlobals::MaximumConnections, &RakNet::SocketDescriptor(NetGlobals::ServerPort, 0), 1);
	assert(startResult == RakNet::RAKNET_STARTED, "Server could not be started!");

	m_serverPeer->SetMaximumIncomingConnections(NetGlobals::MaximumIncomingConnections);

	ServerInfo info;
	memcpy(&info.serverName, serverName.c_str(), serverName.length());
	info.maxPlayers = NetGlobals::MaximumConnections;
	info.connectedPlayers = 0;

	m_serverPeer->SetOfflinePingResponse((const char*)&info, sizeof(info));
	m_processThread = std::thread(&LocalServer::threadedProcess, this);

}

void LocalServer::threadedProcess()
{
	while (!m_shutdownServer) {

		for (RakNet::Packet* packet = m_serverPeer->Receive(); packet; m_serverPeer->DeallocatePacket(packet), packet = m_serverPeer->Receive())
		{
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
				logTrace("[SERVER] New connection from {0}\nAssigned GUID: {1}\n", packet->systemAddress.ToString(), packet->guid.ToString());
			}
			break;

			case ID_DISCONNECTION_NOTIFICATION:
			{
				logTrace("[SERVER] Disconnected {0}\nWith GUID: {1}\n", packet->systemAddress.ToString(), packet->guid.ToString());
			}
			break;

			case ID_CONNECTION_LOST:
			{
				logTrace("[SERVER] Lost connection with {0}\nWith GUID: {1}\n", packet->systemAddress.ToString(), packet->guid.ToString());
			}
			break;

			}

		}

		RakSleep(30);
	}
}

unsigned char LocalServer::getPacketID(RakNet::Packet* p)
{
	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	else
		return (unsigned char)p->data[0];
}
