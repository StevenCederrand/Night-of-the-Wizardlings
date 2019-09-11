#include <Pch/Pch.h>
#include "Client.h"

Client::Client()
{
}

Client::~Client()
{
	RakNet::RakPeerInterface::DestroyInstance(m_clientPeer);
}

void Client::startup()
{
	m_clientPeer = RakNet::RakPeerInterface::GetInstance();
	m_clientPeer->Startup(1, &RakNet::SocketDescriptor(), 1);
}

void Client::connectToAnotherServer(const ServerInfo& server)
{
	bool status = m_clientPeer->Connect(server.serverAddress.ToString(false), server.serverAddress.GetPort(), 0, 0, 0) == RakNet::CONNECTION_ATTEMPT_STARTED;
	assert(status == true, "Client connecting to {0} failed!", server.serverName);
}

void Client::connectToMyServer()
{
	bool status = m_clientPeer->Connect("localhost", NetGlobals::ServerPort, 0, 0, 0) == RakNet::CONNECTION_ATTEMPT_STARTED;
	assert(status == true, "Client connecting to localhost failed!");
}

void Client::process()
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
				logTrace("ID_CONNECTION_REQUEST_ACCEPTED\n");
				m_isConnectedToAnServer = true;
			}
			break;
			
			// print out errors
			case ID_CONNECTION_ATTEMPT_FAILED:
				logTrace("Client Error: ID_CONNECTION_ATTEMPT_FAILED\n");
				m_isConnectedToAnServer = false;
				break;
			
			case ID_ALREADY_CONNECTED:
				logTrace("Client Error: ID_ALREADY_CONNECTED\n");
				break;
			
			case ID_CONNECTION_BANNED:
				logTrace("Client Error: ID_CONNECTION_BANNED\n");
				break;
			
			case ID_INVALID_PASSWORD:
				logTrace("Client Error: ID_INVALID_PASSWORD\n");
				break;
			
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				logTrace("Client Error: ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
				break;
			
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				logTrace("Client Error: ID_NO_FREE_INCOMING_CONNECTIONS\n");
				m_isConnectedToAnServer = false;
				break;
			
			case ID_DISCONNECTION_NOTIFICATION:
				logTrace("ID_DISCONNECTION_NOTIFICATION\n");
				m_isConnectedToAnServer = false;
				break;
			
			case ID_CONNECTION_LOST:
				logTrace("Client Error: ID_CONNECTION_LOST\n");
				m_isConnectedToAnServer = false;
				break;
			
			default:
			{
				logWarning("Unknown packet received!");
			}
			break;
		}
	}
}

const std::vector<ServerInfo>& Client::getServerList() const
{
	return m_serverList;
}

void Client::refreshServerList()
{
	logTrace("Fetching server list...\n");
	m_isRefreshingServerList = true;
	m_serverList.clear();
	findAllServerAddresses();
}

bool Client::doneRefreshingServerList()
{
	return m_isRefreshingServerList;
}

void Client::findAllServerAddresses()
{
	m_clientPeer->Ping("255.255.255.255", NetGlobals::ServerPort, false);

	auto searchTime = RakNet::GetTimeMS() + 2 * 1000; 
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
					RakNet::BitStream bsIn(packet->data + 5, packet->length - 5, false); // Jump 5 bytes
					info = *(ServerInfo*)bsIn.GetData();
					info.serverAddress = packet->systemAddress;
					m_serverList.emplace_back(info);
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