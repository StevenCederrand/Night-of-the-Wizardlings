#ifndef _NETWORK_PLAYERS_H
#define _NETWORK_PLAYERS_H
#include <Pch/Pch.h>
#include <GameObject/WorldObject.h>
#include <Renderer/Renderer.h>

class NetworkPlayers {
public:
	enum FLAG {
		REMOVE,
		ADD,
		NONE
	};

	struct PlayerEntity {
		PlayerPacket data;
		GameObject* gameobject = nullptr; // This is created on the MAIN thread!
		FLAG flag = FLAG::NONE;

	};


	NetworkPlayers();
	~NetworkPlayers();

	void update(const float& dt);
	std::vector<PlayerEntity*>& getPlayersREF();
	

private:
	friend class Client;
	std::mutex m_mutex;
	std::vector<PlayerEntity*> m_players;
	float m_lerpSpeed = 15.f;

};

#endif
