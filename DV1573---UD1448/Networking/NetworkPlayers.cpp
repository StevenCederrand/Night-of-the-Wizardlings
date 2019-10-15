#include <Pch/Pch.h>
#include "NetworkPlayers.h"

glm::vec3 vec3Lerp(const glm::vec3& a, const glm::vec3& b, float t)
{
	glm::vec3 v;
	v.x = (1 - t) * a.x + t * b.x;
	v.y = (1 - t) * a.y + t * b.y;
	v.z = (1 - t) * a.z + t * b.z;

	return v;

}

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
				p->gameobject->loadMesh("TestCube.mesh");
				p->gameobject->setWorldPosition(glm::vec3(1, 0, 0));
				//Submit the player object as a dynamic object
				Renderer::getInstance()->submit(p->gameobject, DYNAMIC); 
			}
			p->flag = FLAG::NONE;
		}
		else if (p->flag == FLAG::REMOVE)
		{

			Renderer::getInstance()->removeDynamic(p->gameobject);
			delete p->gameobject;
			delete p;
			m_players.erase(m_players.begin() + i);
			i--;
			continue;
		}

		GameObject* g = p->gameobject;
		
		if (g != nullptr) {
			glm::vec3 pos = vec3Lerp(p->gameobject->getTransform().position, p->data.position, m_lerpSpeed * dt);
			g->setWorldPosition(pos);
			//g->setTransform(pos, glm::quat(p->data.rotation));
		}
	}
}

std::vector<NetworkPlayers::PlayerEntity*>& NetworkPlayers::getPlayersREF()
{
	return m_players;
}