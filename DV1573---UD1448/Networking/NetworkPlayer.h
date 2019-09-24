#ifndef _NETWORK_PLAYER_H
#define _NETWORK_PLAYER_H
#include <Pch/Pch.h>
#include <GameObject/WorldObject.h>

class LocalServer;
class Client;

class NetworkPlayer
{
	
public:
	NetworkPlayer();
	~NetworkPlayer();
	void updateGameObject();
	const PlayerData& getData() const; // Should only be used for reading.
	void initialize(const std::string& mesh);
	std::string toString() const;
	GameObject* getGameObjectPtr() const;
private:
	friend class LocalServer; // this mean that the local server have access to the private members
	friend class Client;
	PlayerData m_data;
	GameObject* m_gameObject;
	bool m_hasGameObject;
};


#endif
