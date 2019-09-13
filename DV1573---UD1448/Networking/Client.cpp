#include <Pch/Pch.h>
#include "Client.h"

Client::Client()
{
}

Client::~Client()
{
	
	m_shutdownClient = true;
	logTrace("Waiting for client thread to finish...");
	m_processThread.join();
	logTrace("Client process thread shutdown");

	RakNet::RakPeerInterface::DestroyInstance(m_clientPeer);
}

void Client::startup()
{
	m_connectedPlayers.reserve(NetGlobals::MaximumConnections);
	m_clientPeer = RakNet::RakPeerInterface::GetInstance();
	m_clientPeer->Startup(1, &RakNet::SocketDescriptor(), 1);
}

void Client::connectToAnotherServer(const ServerInfo& server)
{
	bool status = m_clientPeer->Connect(server.serverAddress.ToString(false), server.serverAddress.GetPort(), 0, 0, 0) == RakNet::CONNECTION_ATTEMPT_STARTED;
	assert(status == true, "Client connecting to {0} failed!", server.serverName);
	m_processThread = std::thread(&Client::threadedProcess, this);
}

void Client::connectToMyServer()
{
	bool status = m_clientPeer->Connect("localhost", NetGlobals::ServerPort, 0, 0, 0) == RakNet::CONNECTION_ATTEMPT_STARTED;
	assert(status == true, "Client connecting to localhost failed!");
	m_processThread = std::thread(&Client::threadedProcess, this);
}

void Client::threadedProcess()
{
	while (!m_shutdownClient)
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
				logTrace("[CLIENT] Connected to server.\n");
				m_serverAddress = packet->systemAddress;
				m_isConnectedToAnServer = true;
			}
				break;

			case ID_CONNECTION_ATTEMPT_FAILED:
				logTrace("[CLIENT] Connection failed, server might be full.\n");
				m_isConnectedToAnServer = false;
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
				logTrace("[CLIENT] Client Error: incompatible protocol version!\n");
				break;

			case ID_NO_FREE_INCOMING_CONNECTIONS:
				logTrace("[CLIENT] Client Error: No free incoming connection slots!\n");
				m_isConnectedToAnServer = false;
				break;

			case ID_DISCONNECTION_NOTIFICATION:
				logTrace("[CLIENT] Disconnected from server!\n");
				m_isConnectedToAnServer = false;
				break;

			case ID_CONNECTION_LOST:
				logTrace("[CLIENT] Connection to the server is lost!\n");
				m_isConnectedToAnServer = false;
				break;
			case INFO_ABOUT_OTHER_PLAYERS:
			{
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				size_t nrOfConnectedPlayers;
				bsIn.Read(nrOfConnectedPlayers);

				for (size_t i = 0; i < nrOfConnectedPlayers; i++) {
					NetworkPlayer player;
					player.Serialize(false, bsIn);
					m_connectedPlayers.emplace_back(player);
				}
				printAllConnectedPlayers();
				
			}
				break;
			case PLAYER_JOINED:
			{
				logTrace("New player joined!");
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				NetworkPlayer player;
				player.Serialize(false, bsIn);
				m_connectedPlayers.emplace_back(player);
				printAllConnectedPlayers();
				
			}
				break;
			case PLAYER_DISCONNECTED:
			{
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				RakNet::AddressOrGUID guidOfDisconnectedPlayer;
				bsIn.Read(guidOfDisconnectedPlayer);

				bool found = false;
				for (size_t i = 0; i < m_connectedPlayers.size() && !found; i++) {
					if (guidOfDisconnectedPlayer == m_connectedPlayers[i].getData().guid) {
						m_connectedPlayers.erase(m_connectedPlayers.begin() + i);
						found = true;
					}
				}

				logTrace("Player disconnected");
				printAllConnectedPlayers();
			}
				break;
			default:
			{
				logWarning("[CLIENT] Unknown packet received!");
			}
			break;
			}
		}
		RakSleep(30);
	}

	// Client has been told to shutdown here so send a disconnection packet if you're still connected
	if (m_isConnectedToAnServer)
	{
		RakNet::BitStream stream;
		stream.Write((RakNet::MessageID)ID_DISCONNECTION_NOTIFICATION);
		m_clientPeer->Send(&stream,IMMEDIATE_PRIORITY , RELIABLE_ORDERED, 0,m_serverAddress, false);
		RakSleep(250);
	}

}

const std::vector<ServerInfo>& Client::getServerList() const
{
	return m_serverList;
}

const std::vector<NetworkPlayer>& Client::getConnectedPlayers() const
{
	return m_connectedPlayers;
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
/* When we have a UI this will most likely be threaded */
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

					auto byteOffset = packet->length - sizeof(ServerInfo);

					RakNet::BitStream bsIn(packet->data + byteOffset, packet->length - byteOffset, false);
					info = *(ServerInfo*)bsIn.GetData();

					// If the pinged server is full then don't add it to the server list
					if (info.connectedPlayers >= info.maxPlayers) continue;

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

void Client::printAllConnectedPlayers()
{
	for (size_t i = 0; i < m_connectedPlayers.size(); i++) {
		logTrace("Client ({0})\n{3}\n{1}\n{2}", (i + 1), m_connectedPlayers[i].toString(), "________________________", "________________________");
	}
}
