#include <Pch/Pch.h>
#include "NetworkPlayers.h"
#include "Client.h"

NetworkPlayers::NetworkPlayers()
{
}

NetworkPlayers::~NetworkPlayers()
{
	cleanUp();
}

void NetworkPlayers::cleanUp()
{
	for (size_t i = 0; i < m_players.size(); i++)
	{
		PlayerEntity& p = m_players[i];
		delete p.gameobject;
	}
	m_players.clear();
}

void NetworkPlayers::update(const float& dt)
{
	Client::getInstance()->updatePlayersMutexGuard();
	SoundHandler* shPtr = SoundHandler::getInstance();
	for (size_t i = 0; i < m_players.size(); i++)
	{
		PlayerEntity& p = m_players[i];

		if (p.flag == NetGlobals::THREAD_FLAG::ADD)
		{
			if (p.gameobject == nullptr) {
				p.gameobject = new WorldObject();
				p.gameobject->loadMesh("TestCube.mesh");
				p.gameobject->setWorldPosition(glm::vec3(1, 0, 0));
				//Submit the player object as a dynamic object
				Renderer::getInstance()->submit(p.gameobject, DYNAMIC); 
			}
			p.gameobject->setWorldPosition(p.data.position);
			p.flag = NetGlobals::THREAD_FLAG::NONE;
			shPtr->addPlayer(p.data.guid);
		}
		else if (p.flag == NetGlobals::THREAD_FLAG::REMOVE)
		{

			Renderer::getInstance()->removeDynamic(p.gameobject, DYNAMIC);
			delete p.gameobject;
			m_players.erase(m_players.begin() + i);
			shPtr->removePlayer(p.data.guid);
			i--;
			continue;
		}

		if (p.data.inDeflectState)
		{
			shPtr->setSourcePosition(p.data.position, DeflectSound, p.data.guid);
			shPtr->playSound(DeflectSound, p.data.guid);
		}
		
		GameObject* g = p.gameobject;
		
		if (g != nullptr) {
			
			/* Don't render the player if he's dead */
			if (p.data.health <= 0.0f || p.data.hasBeenUpdatedOnce == false)
				g->setShouldRender(false);
			else {
				g->setShouldRender(true);
			}		

			glm::vec3 pos = CustomLerp(g->getTransform().position, p.data.position, m_lerpSpeed * dt);
			
			g->setWorldPosition(pos);
			g->setTransform(pos, glm::quat(p.data.rotation), glm::vec3(1.0f));
		}
	}
}

std::vector<NetworkPlayers::PlayerEntity>& NetworkPlayers::getPlayersREF()
{
	return m_players;
}