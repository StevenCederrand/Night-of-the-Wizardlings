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
	for (size_t i = 0; i < m_players.size(); i++)
	{
		PlayerEntity& p = m_players[i];
		AnimatedObject* animObj = nullptr;
		if (p.flag == NetGlobals::THREAD_FLAG::Add){

			if (p.gameobject == nullptr) {
				p.gameobject = new AnimatedObject("asd");
				p.gameobject->loadMesh("ANIM.mesh");
				p.gameobject->setWorldPosition(glm::vec3(0, 0, 0));
				animObj = dynamic_cast<AnimatedObject*>(p.gameobject);

				if (animObj != nullptr)
				{
					animObj->initAnimations("RunAnimation", 1.0f, 21.0f);
					animObj->initAnimations("IdleAnimation", 22.0f, 93.0f);
					animObj->initAnimations("JumpAnimation", 0.0f, 1.0f);
				}

				//Submit the player object as a dynamic object
				Renderer::getInstance()->submit(p.gameobject, ANIMATEDSTATIC); 
			}
			p.gameobject->setWorldPosition(p.data.position);
			p.flag = NetGlobals::THREAD_FLAG::None;
		}
		else if (p.flag == NetGlobals::THREAD_FLAG::Remove)
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
			else {
				g->setShouldRender(true);
			}
			
			glm::vec3 pos = CustomLerp(g->getTransform().position, p.data.position, m_lerpSpeed * dt);
			
			g->setWorldPosition(pos);
			g->setTransform(pos, glm::quat(p.data.rotation), glm::vec3(1.0f));

			animObj = dynamic_cast<AnimatedObject*>(p.gameobject);
			if (animObj != nullptr) {
				if (p.data.animStates.running == true)
					animObj->playLoopAnimation("RunAnimation");
				if (p.data.animStates.idle == true)
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