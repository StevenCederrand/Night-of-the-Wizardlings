#ifndef _NET_PACKETS_H
#define _NET_PACKETS_H

enum {
	INFO_ABOUT_OTHER_PLAYERS = ID_USER_PACKET_ENUM + 1,
	PLAYER_JOINED,
	PLAYER_DISCONNECTED
};

/* To make sure the compiler aligns the bits */
#pragma pack(push, 1)
struct ServerInfo {
	
	char serverName[16] = { ' ' };
	RakNet::SystemAddress serverAddress;
	unsigned short maxPlayers;
	unsigned short connectedPlayers;
};
#pragma pack(pop)
struct newPlayerInfo {
	RakNet::AddressOrGUID guid;

};

#endif