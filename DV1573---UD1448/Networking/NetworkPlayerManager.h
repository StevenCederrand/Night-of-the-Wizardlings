#ifndef _NETWORK_MANAGER_H
#define _NETWORK_MANAGER_H
#include <Pch/Pch.h>
#include "NetworkPlayer.h"

class NetworkPlayerManager{
public:

	NetworkPlayerManager();
	~NetworkPlayerManager();

	void update(const float& dt);


private:
	std::vector<NetworkPlayer*> m_players;

};


#endif
