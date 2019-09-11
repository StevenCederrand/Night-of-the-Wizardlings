#ifndef _NET_PACKETS_H
#define _NET_PACKETS_H

enum {
	REQUEST_SERVERINFO = ID_USER_PACKET_ENUM + 1,
	RESPONSE_SERVERINFO
};

/* To make sure the compiler aligns the bits */
#pragma pack(push, 1)

struct ServerInfo {
	
	char serverName[16] = { ' ' };
	RakNet::SystemAddress serverAddress;
	unsigned short maxPlayers;
	unsigned short connectedPlayers;

	void Serialize(bool writeToBitstream, RakNet::BitStream* bs)
	{
		bs->Serialize(writeToBitstream, serverName);
		bs->Serialize(writeToBitstream, maxPlayers);
		bs->Serialize(writeToBitstream, connectedPlayers);
	}

};


#pragma pack(pop)


#endif