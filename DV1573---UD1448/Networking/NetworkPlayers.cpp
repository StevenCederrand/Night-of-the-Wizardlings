#include <Pch/Pch.h>
#include "NetworkPlayers.h"
#include "Client.h"

NetworkPlayers::NetworkPlayers()
{
	m_displayScale = glm::vec2(2.0f, 2.0f);
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
		delete p.healthDisplay;
		delete p.gameobject;
		
		if(p.nameplate != nullptr)
			TextManager::getInstance()->removeText(p.nameplate->getUniqueIndex());
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

				p.healthDisplay = new WorldHudObject("Assets/Textures/hud/tmpHP.png", p.data.position, m_displayScale);
				
				

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
			delete p.healthDisplay;
			delete p.gameobject;
			TextManager::getInstance()->removeText(p.nameplate->getUniqueIndex());
			m_players.erase(m_players.begin() + i);
			shPtr->removePlayer(p.data.guid);
			i--;
			continue;
		}

		GameObject* g = p.gameobject;
		if (p.data.inDeflectState && p.data.health > 0.0f)
		{
			shPtr->setSourcePosition(p.data.position, DeflectSound, p.data.guid);
			GameObject* shieldObject = new EnemyShieldObject("enemyShield");
			//logTrace(std::to_string(p.data.position.x) + " " + std::to_string(p.data.position.y) + " " + std::to_string(p.data.position.z));
			shieldObject->loadMesh("EnemyShieldMesh.mesh");
			
			glm::vec3 spawnpos = p.data.position + glm::vec3(0.0f, p.data.meshHalfSize.y, 0.0f);
			glm::vec3 newShieldpos = g->getObjectTransform().position + glm::vec3(0.0f, p.data.meshHalfSize.y, 0.0f);
			glm::vec3 shieldLerp = CustomLerp(newShieldpos, spawnpos, m_lerpSpeed * dt);
			shieldObject->setTransform(shieldLerp, p.data.rotation, glm::vec3(1.0));
			Renderer::getInstance()->submit(shieldObject, ENEMY_SHIELD);
			if (p.data.hasDeflectMana)
			{
				if (!p.wasDeflecting)
				{
					p.deflectSoundGain = shPtr->getMasterVolume(); // Will automaticially be set relative to master sound
					shPtr->setSourceGain(p.deflectSoundGain, DeflectSound, p.data.guid);					
					shPtr->playSound(DeflectSound, p.data.guid);
					p.wasDeflecting = true;
				}
			}
			//No mana
			else
			{
				p.deflectSoundGain -= 3.0f * shPtr->getMasterVolume() * dt;
				if (p.deflectSoundGain > 0.0f)
				{
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
			p.deflectSoundGain -= 3.0f * shPtr->getMasterVolume() * dt;
			if (p.deflectSoundGain > 0.0f)
			{				
				shPtr->setSourceGain(p.deflectSoundGain, DeflectSound, p.data.guid);
			}
			else
			{
				shPtr->stopSound(DeflectSound, p.data.guid);
				p.deflectSoundGain = shPtr->getMasterVolume(); // Will automaticially be set relative to master sound
				shPtr->setSourceGain(p.deflectSoundGain, DeflectSound, p.data.guid);
				p.wasDeflecting = false;			
			}
		}		

		if (g != nullptr) {

			/* Don't render the player if he's dead */
			if (p.data.health <= 0.0f || p.data.hasBeenUpdatedOnce == false) {
				p.healthDisplay->setShouldRender(false);
				g->setShouldRender(false);
				if (p.nameplate != nullptr) {
					p.nameplate->setShouldRender(false);
				}
			}

			else {
				p.healthDisplay->setShouldRender(true);
				g->setShouldRender(true);
				if (p.nameplate != nullptr) {
					p.nameplate->setShouldRender(true);
				}
			}

			if (p.playerFlag == NetGlobals::THREAD_PLAYER_FLAG::SafeToAddNameplate && p.nameplate == nullptr) {
				p.nameplate = TextManager::getInstance()->addDynamicText(std::string(p.data.userName),
					1.0f,
					p.data.position + glm::vec3(0.0f, p.data.meshHalfSize.y * 1.2f, 0.0f),
					0.0f,
					TextManager::TextBehaviour::StayForever);
				p.nameplate->setToFaceCamera(true);
				p.nameplate->setIgnoreDepthTest(false);
				p.playerFlag == NetGlobals::THREAD_PLAYER_FLAG::AlreadyAdded;
			}

			glm::vec3 pos = CustomLerp(g->getObjectTransform().position, p.data.position, m_lerpSpeed * dt);


			float healthClip = static_cast<float>(p.data.health) / static_cast<float>(NetGlobals::PlayerMaxHealth);
			p.healthDisplay->setXClip(healthClip);
			p.healthDisplay->setCenter(pos + glm::vec3(0.0f, p.data.meshHalfSize.y * 2.0f, 0.0f));
			
			if (p.nameplate != nullptr) {
				p.nameplate->setPosition(pos + glm::vec3(0.0f, p.data.meshHalfSize.y * 2.20, 0.0f));
				float distance = glm::distance(p.nameplate->getPosition(), Client::getInstance()->getMyData().position);
				p.nameplate->setScale(fminf(fmaxf(distance / 8.0f, 1.0f), 2.0f));

			}

			Renderer::getInstance()->submitWorldHud(p.healthDisplay);
			
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
