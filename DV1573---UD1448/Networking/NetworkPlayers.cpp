#include <Pch/Pch.h>
#include "NetworkPlayers.h"


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
	std::lock_guard<std::mutex> lockGuard(m_mutex);
	for (size_t i = 0; i < m_players.size(); i++)
	{
		PlayerEntity& p = m_players[i];
		AnimatedObject* animObj = dynamic_cast<AnimatedObject*>(p.gameobject);

		if (p.flag == NetGlobals::THREAD_FLAG::ADD)
		{
			if (p.gameobject == nullptr) {
				p.gameobject = new AnimatedObject("asd");
				p.gameobject->loadMesh("ANIM.mesh");
				p.gameobject->setWorldPosition(glm::vec3(0, 0, 0));
				
				if (animObj != nullptr)
				{
					animObj->initAnimations("RunAnimation", 3.0f, 20.0f);
					animObj->initAnimations("IdleAnimation", 1.0f, 2.0f);

				}

				//Submit the player object as a dynamic object
				Renderer::getInstance()->submit(p.gameobject, ANIMATEDSTATIC); 
			}
			p.gameobject->setWorldPosition(p.data.position);
			p.flag = NetGlobals::THREAD_FLAG::NONE;
		}
		else if (p.flag == NetGlobals::THREAD_FLAG::REMOVE)
		{

			Renderer::getInstance()->removeDynamic(p.gameobject, DYNAMIC);
			delete p.gameobject;
			m_players.erase(m_players.begin() + i);
			i--;
			continue;
		}

		GameObject* g = p.gameobject;
		
		
		if (g != nullptr) {
			
			/* Don't render the player if he's dead */
			if (p.data.health <= 0.0f || p.data.hasBeenUpdatedOnce == false)
				g->setShouldRender(false);
			else
				g->setShouldRender(true);

			

			glm::vec3 pos = CustomLerp(g->getTransform().position, p.data.position, m_lerpSpeed * dt);
			g->setWorldPosition(pos);
			g->setTransform(pos, glm::quat(p.data.rotation), glm::vec3(1.0f));

			if (animObj != nullptr) {
				if (p.data.animStates.running == true)
				{
					logTrace("e du h�er");
					animObj->playLoopAnimation("RunAnimation");

				}
				else
				{
					animObj->playLoopAnimation("IdleAnimation");
				}

			}
			g->update(dt);

		}
	}
}

std::vector<NetworkPlayers::PlayerEntity>& NetworkPlayers::getPlayersREF()
{
	return m_players;
}