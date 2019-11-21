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
	
	SoundHandler* shPtr = SoundHandler::getInstance();
	std::lock_guard<std::mutex> lockGuard(NetGlobals::UpdatePlayersMutex);
	for (size_t i = 0; i < m_players.size(); i++)
	{
		PlayerEntity& p = m_players[i];
		AnimatedObject* animObj = nullptr;
		if (p.flag == NetGlobals::THREAD_FLAG::Add){

			if (p.gameobject == nullptr) {

				p.gameobject = new AnimatedObject("asd");
				p.gameobject->loadMesh("NyCharacter.mesh");
				p.gameobject->setWorldPosition(glm::vec3(0, 0, 0));
				animObj = dynamic_cast<AnimatedObject*>(p.gameobject);

				if (animObj != nullptr)
				{
					animObj->initAnimations("RunAnimation", 2.0f, 22.0f);
					animObj->initAnimations("IdleAnimation", 25.0f, 93.0f);
					animObj->initAnimations("CastAnimation", 95.0f, 109.0f);
					//animObj->initAnimations("JumpAnimation", 1.0f, 21.0f);

				}
				std::cout << "NetworkPlayer @memAddr: " << p.gameobject << "\n";
				//Submit the player object as a dynamic object
				Renderer::getInstance()->submit(p.gameobject, ANIMATEDSTATIC); 
			}
			p.gameobject->setWorldPosition(p.data.position);
			
			shPtr->addPlayer(p.data.guid);
			p.flag = NetGlobals::THREAD_FLAG::None;
		}
		else if (p.flag == NetGlobals::THREAD_FLAG::Remove)
		{
			Renderer::getInstance()->removeRenderObject(p.gameobject, ANIMATEDSTATIC);

			delete p.gameobject;
			m_players.erase(m_players.begin() + i);
			shPtr->removePlayer(p.data.guid);
			i--;
			continue;
		}

		//Sound stuff

		/*for (int j = 0; j < NR_OF_SUBSEQUENT_SOUNDS; j++)
		{
			if (shPtr->getSourceState(FireSound, p.data.guid, j) == AL_PLAYING)
			{
				shPtr->setSourcePosition(p.data.FireSound)
			}
		}*/
		

		if (p.data.inDeflectState)
		{
			if (!p.wasDeflecting)
			{
				//kolla position uppdatering
				shPtr->setSourcePosition(p.data.position, DeflectSound, p.data.guid);
				shPtr->playSound(DeflectSound, p.data.guid);
				p.wasDeflecting = true;
			}
			
			//Just fading out until we know we are out of mana on the network
			if (p.wasDeflecting)
			{
				if (p.deflectSoundGain > 0.0f)
				{
					p.deflectSoundGain -= 0.2 * dt;
					shPtr->setSourceGain(p.deflectSoundGain, DeflectSound, p.data.guid);
				}
				else
				{
					shPtr->stopSound(DeflectSound, p.data.guid);					
				}
			}
		}
		else if (p.wasDeflecting)
		{
			if (p.deflectSoundGain > 0.0f)
			{
				p.deflectSoundGain -= 0.3 * dt;
				shPtr->setSourceGain(p.deflectSoundGain, DeflectSound, p.data.guid);
			}
			else
			{
				shPtr->stopSound(DeflectSound, p.data.guid);
				p.deflectSoundGain = 1.0f;
				shPtr->setSourceGain(p.deflectSoundGain, DeflectSound, p.data.guid);
				p.wasDeflecting = false;
			}						
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
				if (p.data.animStates.jumping == true)
				{
					//animObj->playAnimation("JumpAnimation");
					
					shPtr->setSourcePosition(p.data.position, JumpSound, p.data.guid, 0);
					//Just in case two sounds are playing at once, set the position of the second sound aswell
					//We could check which sources are playing but we know there are not going
					//to be playing more than two jump sounds at once
					shPtr->setSourcePosition(p.data.position, JumpSound, p.data.guid, 1);					
					shPtr->playSound(JumpSound, p.data.guid);	
					p.isJumping = true;
				}	
				if (p.isJumping && p.data.onGround)
				{
					shPtr->setSourcePosition(p.data.position, LandingSound, p.data.guid, 0);
					shPtr->setSourcePosition(p.data.position, LandingSound, p.data.guid, 1);
					shPtr->playSound(LandingSound, p.data.guid);
					p.isJumping = false;
				}

				if (p.data.animStates.casting == true)
				{
					animObj->playAnimation("CastAnimation");
				}
				//if (p.data.animStates.deflecting == true)
				//{
				//	animObj->playAnimation("CastAnimation");
				//}
				if (p.data.animStates.running == true)
				{
					animObj->playLoopAnimation("RunAnimation");
					
					if (!p.wasRunning)
					{							
						shPtr->setSourcePosition(p.data.position, StepsSound, p.data.guid);
						shPtr->playSound(StepsSound, p.data.guid);	
						shPtr->setSourceLooping(true, StepsSound, p.data.guid);
						p.wasRunning = true;
					}
				}				
				if (p.data.animStates.idle == true)
				{					
					if (p.wasRunning == true)
					{						
						shPtr->stopSound(StepsSound, p.data.guid);
						p.wasRunning = false;
					}
					animObj->playLoopAnimation("IdleAnimation");
				}
				if (p.wasRunning == true)
				{					
					shPtr->setSourcePosition(p.data.position, StepsSound, p.data.guid);
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