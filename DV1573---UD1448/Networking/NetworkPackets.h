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
	SPELL_ALL_EXISTING_SPELLS,
	SPELL_CREATED,
	SPELL_UPDATE,
	SPELL_DESTROY,
	SPELL_PLAYER_HIT,
	GAME_START_COUNTDOWN,
	GAME_ROUND_TIMER,
	RESPAWN_TIME,
	RESPAWN_PLAYER,
	SCORE_UPDATE,
	SPELL_GOT_DEFLECTED,
	PICKUP_CREATED,
	PICKUP_REMOVED,
	PICKUP_NOTIFICATION,
	HEAL_BUFF,
	DAMAGE_BUFF_ACTIVE,
	DAMAGE_BUFF_INACTIVE,
	KILL_FEED
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
	RakNet::AddressOrGUID lastHitByGuid;
	uint32_t timestamp = 0;
	int health = NetGlobals::maxPlayerHealth;
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 lookDirection = glm::vec3(0.0f);
	glm::vec3 latestSpawnPosition = glm::vec3(0.0f);
	char userName[16] = { ' ' };
	int numberOfKills = 0;
	int numberOfDeaths = 0;
	bool inDeflectState = false;
	bool hasBeenUpdatedOnce = false;
	bool hasDamageBuff = false;

	void Serialize(bool writeToStream, RakNet::BitStream& stream)
	{
		stream.Serialize(writeToStream, guid);
		stream.Serialize(writeToStream, lastHitByGuid);
		stream.Serialize(writeToStream, timestamp);
		stream.Serialize(writeToStream, health);
		stream.Serialize(writeToStream, position);
		stream.Serialize(writeToStream, rotation);
		stream.Serialize(writeToStream, lookDirection);
		stream.Serialize(writeToStream, latestSpawnPosition);
		stream.Serialize(writeToStream, userName);
		stream.Serialize(writeToStream, numberOfKills);
		stream.Serialize(writeToStream, numberOfDeaths);
		stream.Serialize(writeToStream, inDeflectState);
		stream.Serialize(writeToStream, hasBeenUpdatedOnce);
		stream.Serialize(writeToStream, hasDamageBuff);
	}
};

struct CountdownPacket {
	uint32_t timeLeft;

	void Serialize(bool writeToStream, RakNet::BitStream& stream)
	{
		stream.Serialize(writeToStream, timeLeft);
	}
};

struct RoundTimePacket {

	uint32_t minutes;
	uint32_t seconds;

	void Serialize(bool writeToStream, RakNet::BitStream& stream)
	{
		stream.Serialize(writeToStream, minutes);
		stream.Serialize(writeToStream, seconds);
	}

};

struct KillFeedPacket {
	RakNet::AddressOrGUID killerGuid;
	RakNet::AddressOrGUID deadGuid;

	void Serialize(bool writeToStream, RakNet::BitStream& stream)
	{
		stream.Serialize(writeToStream, killerGuid);
		stream.Serialize(writeToStream, deadGuid);
	}
};

struct SpellPacket{
	RakNet::MessageID packetType;
	uint64_t SpellID = 0;
	uint32_t timestamp = 0;
	RakNet::RakNetGUID CreatorGUID = RakNet::UNASSIGNED_RAKNET_GUID;
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Rotation = glm::vec3(0.0f);
	glm::vec3 Scale = glm::vec3(1.0f);
	glm::vec3 Direction = glm::vec3(0.0f);
	SPELL_TYPE SpellType = SPELL_TYPE::UNKNOWN;


	void Serialize(bool writeToStream, RakNet::BitStream& stream) {
		stream.Serialize(writeToStream, packetType);
		stream.Serialize(writeToStream, SpellID);
		stream.Serialize(writeToStream, timestamp);
		stream.Serialize(writeToStream, CreatorGUID);
		stream.Serialize(writeToStream, Position);
		stream.Serialize(writeToStream, Rotation);
		stream.Serialize(writeToStream, Scale);
		stream.Serialize(writeToStream, Direction);
		stream.Serialize(writeToStream, SpellType);
	}


};

struct HitPacket {

	uint64_t SpellID = 0;
	RakNet::RakNetGUID CreatorGUID = RakNet::UNASSIGNED_RAKNET_GUID;
	RakNet::RakNetGUID playerHitGUID = RakNet::UNASSIGNED_RAKNET_GUID;
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Scale = glm::vec3(1.0f);
	glm::vec3 SpellDirection = glm::vec3(0.0f);
	glm::quat Rotation = glm::quat();
	float damage = 0.0f;
	SPELL_TYPE SpellType = SPELL_TYPE::UNKNOWN;

	void Serialize(bool writeToStream, RakNet::BitStream& stream) {
		stream.Serialize(writeToStream, SpellID);
		stream.Serialize(writeToStream, CreatorGUID);
		stream.Serialize(writeToStream, playerHitGUID);
		stream.Serialize(writeToStream, Position);
		stream.Serialize(writeToStream, Scale);
		stream.Serialize(writeToStream, SpellDirection);
		stream.Serialize(writeToStream, Rotation);
		stream.Serialize(writeToStream, damage);
		stream.Serialize(writeToStream, SpellType);
	}


};

struct PickupPacket {
	char locationName[16] = { ' ' };
	uint64_t uniqueID;
	glm::vec3 position;
	PickupType type;

	void Serialize(bool writeToStream, RakNet::BitStream& stream) {
		stream.Serialize(writeToStream, locationName);
		stream.Serialize(writeToStream, uniqueID);
		stream.Serialize(writeToStream, position);
		stream.Serialize(writeToStream, type);
	}
};

struct ServerStateChange {
	NetGlobals::SERVER_STATE currentState;

	void Serialize(bool writeToStream, RakNet::BitStream& stream) {
		stream.Serialize(writeToStream, currentState);
	}
};

#endif