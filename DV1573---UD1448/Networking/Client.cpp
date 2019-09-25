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
		m_connectedPlayers.reserve(NetGlobals::MaximumConnections);
		m_clientPeer = RakNet::RakPeerInterface::GetInstance();
		m_clientPeer->Startup(1, &RakNet::SocketDescriptor(), 1);
		m_initialized = true;

		
	}
}

void Client::destroy()
{
	if (m_initialized) {
		m_shutdownThread = true;
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

	bool status = m_clientPeer->Connect(server.serverAddress.ToString(false), server.serverAddress.GetPort(), 0, 0, 0) == RakNet::CONNECTION_ATTEMPT_STARTED;
	assert((status == true, "Client connecting to {0} failed!", server.serverName));

	if (m_processThread.joinable())
		m_processThread.join();

	m_processThread = std::thread(&Client::threadedProcess, this);
	
}

void Client::connectToMyServer()
{
	m_failedToConnect = false;
	m_shutdownThread = false;
	m_isConnectedToAnServer = false;

	bool status = m_clientPeer->Connect("localhost", NetGlobals::ServerPort, 0, 0, 0) == RakNet::CONNECTION_ATTEMPT_STARTED;
	assert((status == true, "Client connecting to localhost failed!"));
	
	if (m_processThread.joinable())
		m_processThread.join();

	
	m_processThread = std::thread(&Client::threadedProcess, this);
}

void Client::threadedProcess()
{
	while (!m_shutdownThread)
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
				m_playerData.guid = m_clientPeer->GetMyGUID();
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
			case INFO_ABOUT_OTHER_PLAYERS:
			{
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				size_t nrOfConnectedPlayers;
				bsIn.Read(nrOfConnectedPlayers);

				for (size_t i = 0; i < nrOfConnectedPlayers; i++) {
					PlayerData player;
					player.Serialize(false, bsIn);
					m_connectedPlayers.emplace_back(player);

					std::lock_guard<std::mutex> lockGuard(m_playerEntities.m_mutex);
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
				PlayerData player;
				player.Serialize(false, bsIn);
				m_connectedPlayers.emplace_back(player);
				

				std::lock_guard<std::mutex> lockGuard(m_playerEntities.m_mutex);
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
			case PLAYER_DATA:
			{
				//logTrace("Player data received from server");
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				PlayerData pData;
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

			default:
			{
				logWarning("[CLIENT] Unknown packet received!");
			}
			break;
			}
		}

		updateDataOnServer();
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

void Client::updatePlayerData(Player* player)
{
	m_playerData.health = player->getHealth();
	m_playerData.position = player->getPlayerPos();
}

void Client::updateNetworkedPlayers(const float& dt)
{
	if(m_initialized && m_isConnectedToAnServer)
		m_playerEntities.update(dt);
}

void Client::updateDataOnServer()
{
	// Player data sent to server
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)PLAYER_DATA);
	m_playerData.Serialize(true, bsOut);
	m_clientPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
}


const std::vector<std::pair<unsigned int, ServerInfo>>& Client::getServerList() const
{
	return m_serverList;
}

const std::vector<PlayerData>& Client::getConnectedPlayers() const
{
	return m_connectedPlayers;
}

NetworkPlayers& Client::getNetworkPlayersREF()
{
	return m_playerEntities;
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

					// If the pinged server is full then don't add it to the server list
					if (info.connectedPlayers >= info.maxPlayers) continue;

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

void Client::printAllConnectedPlayers()
{
	/*for (size_t i = 0; i < m_connectedPlayers.size(); i++) {
		logTrace("Client ({0})\n{3}\n{1}\n{2}", (i + 1), m_connectedPlayers[i]->toString(), "________________________", "________________________");
	}*/
}
