#ifndef _NETWORK_PLAYER_H
#define _NETWORK_PLAYER_H
#include <Pch/Pch.h>

class LocalServer;
class Client;

class NetworkPlayer
{

public:
	struct Data {
		RakNet::AddressOrGUID guid;
		float health;
		glm::vec3 position;
		glm::vec3 rotation;
	};
public:
	NetworkPlayer();
	~NetworkPlayer();
	const Data& getData() const; // Should only be used for reading.
	void Serialize(bool writeToStream, RakNet::BitStream& stream);
	std::string toString() const;

private:
	friend class LocalServer; // this mean that the local server have access to the private members
	friend class Client;
	Data m_data;

};


#endif
