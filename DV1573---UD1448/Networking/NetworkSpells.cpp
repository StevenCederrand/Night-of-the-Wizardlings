#include <Pch/Pch.h>
#include "NetworkSpells.h"
#include "Client.h" 

NetworkSpells::NetworkSpells()
{
}

NetworkSpells::~NetworkSpells()
{
	cleanUp();
}

void NetworkSpells::update(const float& dt)
{
	SoundHandler* shPtr = SoundHandler::getInstance();

	if (Client::getInstance()->isConnectedToSever()) {
		
		std::lock_guard<std::mutex> lockGuard(NetGlobals::UpdateSpellsMutex);
		
		for (size_t i = 0; i < m_entities.size(); i++) {
		
			SpellEntity& e = m_entities[i];			
			
			if (e.flag == NetGlobals::THREAD_FLAG::Add) {
		
				if (e.gameobject == nullptr) {
					
					if (e.spellData.SpellType == OBJECT_TYPE::NORMALATTACK || e.spellData.SpellType == OBJECT_TYPE::UNKNOWN) {
						e.gameobject = new AttackSpell(e.spellData.Position);
					}
					else if (e.spellData.SpellType == OBJECT_TYPE::ENHANCEATTACK) {
						e.gameobject = new AttackSpell(e.spellData.Position);
					}
					else if (e.spellData.SpellType == OBJECT_TYPE::REFLECT) {
						e.gameobject = new ReflectSpell(e.spellData.Position);
					}
					else if (e.spellData.SpellType == OBJECT_TYPE::FLAMESTRIKE) {
						e.gameobject = new AOEAttack(e.spellData.Position);						
						if((e.spellData.SoundSlot = shPtr->playSound(FireSound, e.spellData.CreatorGUID)) != -1)
							shPtr->setSourcePosition(e.spellData.Position, FireSound, e.spellData.CreatorGUID, e.spellData.SoundSlot);
					}
					else if (e.spellData.SpellType == OBJECT_TYPE::FIRE) {
						e.gameobject = new fire(e.spellData.Position);
						shPtr->setSourcePosition(e.spellData.Position, GlassBreakSound, e.spellData.CreatorGUID);
						shPtr->playSound(GlassBreakSound, e.spellData.CreatorGUID);
					}
					else {
						return;
					}
					
					e.gameobject->setWorldPosition(e.spellData.Position);
					Renderer::getInstance()->submit(e.gameobject, SPELL);
					e.flag = NetGlobals::THREAD_FLAG::None;

				}
			}
			else if (e.flag == NetGlobals::THREAD_FLAG::Remove)
			{
				Renderer::getInstance()->removeRenderObject(e.gameobject, SPELL);
				delete e.gameobject;
				m_entities.erase(m_entities.begin() + i);
				i--;
				continue;
			}
			else if (e.flag == NetGlobals::THREAD_FLAG::None)
			{			
				if (e.spellData.SpellType == OBJECT_TYPE::FLAMESTRIKE)
				{
					if(e.spellData.SoundSlot != -1)
						shPtr->setSourcePosition(e.spellData.Position, FireSound, e.spellData.CreatorGUID, e.spellData.SoundSlot);
				}
				
			}

			GameObject* g = e.gameobject;

			if (g != nullptr) {
				
				glm::vec3 pos = CustomLerp(g->getTransform().position, e.spellData.Position, m_lerpSpeed * dt);
				g->setWorldPosition(pos);

			}
		}
	}
}

void NetworkSpells::cleanUp()
{
	for (size_t i = 0; i < m_entities.size(); i++) {
		if (m_entities[i].gameobject != nullptr)
			delete m_entities[i].gameobject;
	}
	m_entities.clear();
}

std::vector<NetworkSpells::SpellEntity>& NetworkSpells::getSpellEntitiesREF()
{
	return m_entities;
}

