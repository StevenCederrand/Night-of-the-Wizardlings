#ifndef _NET_PACKETS_H
#define _NET_PACKETS_H
#include <Spells/SpellTypes.h>

enum {
	INFO_ABOUT_OTHER_PLAYERS = ID_USER_PACKET_ENUM + 1,
	PLAYER_ACCEPTED_TO_SERVER,
	PLAYER_NOT_ACCEPTED_TO_SERVER,
	PLAYER_JOINED,
	PLAYER_DISCONNECTED,
	PLAYER_UPDATE_PACKET,
	SERVER_CURRENT_STATE,
	SERVER_CHANGE_STATE,
	ADMIN_PACKET,
	SPELL_CREATED,
	SPELL_UPDATE,
	SPELL_DESTROY,
	SPELL_PLAYER_HIT
};

/* To make sure the compiler aligns the bits */
#pragma pack(push, 1)
struct ServerInfo {
	
	char serverName[16] = { ' ' };
	RakNet::SystemAddress serverAddress;
	unsigned short maxPlayers;
	unsigned short connectedPlayers;
	NetGlobals::SERVER_STATE currentState;
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

struct SpellPacket{
	SpellPacket() {}
	RakNet::MessageID packetType;
	uint64_t SpellID = 0;
	RakNet::RakNetGUID CreatorGUID = RakNet::UNASSIGNED_RAKNET_GUID;
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Rotation = glm::vec3(0.0f);
	SPELL_TYPE SpellType = SPELL_TYPE::UNKNOWN;

	void Serialize(bool writeToStream, RakNet::BitStream& stream) {
		stream.Serialize(writeToStream, packetType);
		stream.Serialize(writeToStream, SpellID);
		stream.Serialize(writeToStream, CreatorGUID);
		stream.Serialize(writeToStream, Position);
		stream.Serialize(writeToStream, Rotation);
		stream.Serialize(writeToStream, SpellType);
	}

	std::string toString() const {
		std::string str = "---Spell Packet---\n";
		str +=	"Spell ID: " + std::to_string(SpellID) + "\n";
		str +=	"Creator ID: " + std::string(CreatorGUID.ToString()) + "\n" ;
		str += "Position: (" + std::to_string(Position.x) + ", " + std::to_string(Position.y) + ", " + std::to_string(Position.z) + ")\n";
		str += "Rotation: (" + std::to_string(Rotation.x) + ", " + std::to_string(Rotation.y) + ", " + std::to_string(Rotation.z) + ")\n";
		
		if(SpellType == SPELL_TYPE::ENHANCEATTACK)
			str += "SpellType: Enhanced attack";
		else if (SpellType == SPELL_TYPE::NORMALATTACK)
			str += "SpellType: Normal attack";
		else if (SpellType == SPELL_TYPE::ENHANCEATTACK)
			str += "SpellType: Unknown type";
		

		return str;
	}
};

struct ServerStateChange {
	NetGlobals::SERVER_STATE currentState;

	void Serialize(bool writeToStream, RakNet::BitStream& stream) {
		stream.Serialize(writeToStream, currentState);
	}
};

#endif