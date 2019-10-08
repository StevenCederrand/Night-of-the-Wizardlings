#ifndef _NET_PACKETS_H
#define _NET_PACKETS_H
#include <Spells/SpellTypes.h>

enum {
	INFO_ABOUT_OTHER_PLAYERS = ID_USER_PACKET_ENUM + 1,
	PLAYER_JOINED,
	PLAYER_DISCONNECTED,
	PLAYER_UPDATE_PACKET,
	SERVER_CURRENT_STATE,
	SERVER_CHANGE_STATE,
	ADMIN_PACKET,
	SPELL_CREATED,
	SPELL_UPDATE
};

/* To make sure the compiler aligns the bits */
#pragma pack(push, 1)
struct ServerInfo {
	
	char serverName[16] = { ' ' };
	RakNet::SystemAddress serverAddress;
	unsigned short maxPlayers;
	unsigned short connectedPlayers;
	NetGlobals::ServerState currentState;
};
#pragma pack(pop)
struct newPlayerInfo {
	RakNet::AddressOrGUID guid;
};

struct PlayerPacket {
	RakNet::AddressOrGUID guid;
	int health = 100;
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);


	void Serialize(bool writeToStream, RakNet::BitStream& stream)
	{
		stream.Serialize(writeToStream, guid);
		stream.Serialize(writeToStream, health);
		stream.Serialize(writeToStream, position);
		stream.Serialize(writeToStream, rotation);
	}
};

struct SpellPacket : public RakNet::NetworkIDObject {
	RakNet::RakNetGUID SpellGUID = RakNet::UNASSIGNED_RAKNET_GUID;
	RakNet::RakNetGUID CreatorGUID = RakNet::UNASSIGNED_RAKNET_GUID;
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Rotation = glm::vec3(0.0f);
	SPELL_TYPE SpellType = SPELL_TYPE::UNKNOWN;

	void Serialize(bool writeToStream, RakNet::BitStream& stream) {
		stream.Serialize(writeToStream, SpellGUID);
		stream.Serialize(writeToStream, CreatorGUID);
		stream.Serialize(writeToStream, Position);
		stream.Serialize(writeToStream, Rotation);
		stream.Serialize(writeToStream, SpellType);
	}
};

struct ServerStateChange {
	NetGlobals::ServerState currentState;

	void Serialize(bool writeToStream, RakNet::BitStream& stream) {
		stream.Serialize(writeToStream, currentState);
	}
};

#endif