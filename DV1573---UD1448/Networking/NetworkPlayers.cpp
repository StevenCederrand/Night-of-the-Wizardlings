#include <Pch/Pch.h>
#include "NetworkPlayers.h"

NetworkPlayers::NetworkPlayers()
{
}

NetworkPlayers::~NetworkPlayers()
{
	for (size_t i = 0; i < m_players.size(); i++)
	{
		PlayerEntity* p = m_players[i];
		delete p->gameobject;
		delete p;
	}
	m_players.clear();
}

void NetworkPlayers::update(const float& dt)
{
	std::lock_guard<std::mutex> lockGuard(m_mutex);
	for (size_t i = 0; i < m_players.size(); i++)
	{
		PlayerEntity* p = m_players[i];

		if (p->flag == FLAG::ADD)
		{
			if (p->gameobject == nullptr) {
				p->gameobject = new WorldObject();
				p->gameobject->loadMesh("SexyCube.mesh");
			}
			p->flag = FLAG::NONE;
		}
		else if (p->flag == FLAG::REMOVE)
		{
			delete p->gameobject;
			delete p;
			m_players.erase(m_players.begin() + i);
			i--;
			continue;
		}

		GameObject* g = p->gameobject;
		
		if(g != nullptr)
			g->setTransform(p->data.position, glm::quat(p->data.rotation));

	}

}

std::vector<NetworkPlayers::PlayerEntity*>& NetworkPlayers::getPlayersREF()
{
	return m_players;
}
