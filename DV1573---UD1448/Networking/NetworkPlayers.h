#ifndef _NETWORK_PLAYERS_H
#define _NETWORK_PLAYERS_H
#include <Pch/Pch.h>
#include <GameObject/WorldObject.h>
#include <Renderer/Renderer.h>

class NetworkPlayers {
public:
	struct PlayerEntity {
		PlayerPacket data;
		GameObject* gameobject = nullptr;
		NetGlobals::THREAD_FLAG flag = NetGlobals::THREAD_FLAG::None;
		float deflectSoundGain = 1.0f;
		bool correctPositionOnFirstUpdate = false;	
		bool wasRunning = false;		
		bool isJumping = false;
		bool wasDeflecting = false;
	};

	NetworkPlayers();
	~NetworkPlayers();
	void cleanUp();
	void update(const float& dt);
	std::vector<PlayerEntity>& getPlayersREF();
	

private:
	friend class Client;
	std::vector<PlayerEntity> m_players;
	float m_lerpSpeed = 15.f;

};

#endif
