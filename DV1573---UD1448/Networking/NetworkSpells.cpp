#include <Pch/Pch.h>
#include "NetworkSpells.h"
#include "Client.h"

NetworkSpells::NetworkSpells()
{
}

NetworkSpells::~NetworkSpells()
{
	for (size_t i = 0; i < m_entities.size(); i++) {
		if (m_entities[i].gameobject != nullptr)
			delete m_entities[i].gameobject;
	}

}

void NetworkSpells::update(const float& dt)
{
	if (Client::getInstance()->isConnectedToSever()) {
		
		std::lock_guard<std::mutex> lockGuard(m_mutex);
		
		for (size_t i = 0; i < m_entities.size(); i++) {
		
			SpellEntity& e = m_entities[i];
			
			if (e.flag == NetGlobals::THREAD_FLAG::ADD) {
				if (e.gameobject == nullptr) {
					
					e.gameobject = new WorldObject();
					
					if (e.spellData.SpellType == SPELL_TYPE::NORMALATTACK || e.spellData.SpellType == SPELL_TYPE::UNKNOWN) {
						e.gameobject->loadMesh("TestSphere.mesh");
					}
					else if (e.spellData.SpellType == SPELL_TYPE::ENHANCEATTACK) {
						e.gameobject->loadMesh("TestCube.mesh");
					}
				
					e.gameobject->setWorldPosition(e.spellData.Position);
					Renderer::getInstance()->submit(e.gameobject, SPELL);
					e.flag = NetGlobals::THREAD_FLAG::NONE;

				}
			}
			else if (e.flag == NetGlobals::THREAD_FLAG::REMOVE)
			{
				Renderer::getInstance()->removeDynamic(e.gameobject, SPELL);
				delete e.gameobject;
				m_entities.erase(m_entities.begin() + i);
				i--;
				continue;
			}

			GameObject* g = e.gameobject;

			if (g != nullptr) {
				glm::vec3 pos = CustomLerp(g->getTransform().position, e.spellData.Position, m_lerpSpeed * dt);
				g->setWorldPosition(pos);
				//g->setTransform(pos, glm::quat(p->data.rotation));

			}
		}
	}
}

std::vector<NetworkSpells::SpellEntity>& NetworkSpells::getSpellEntitiesREF()
{
	return m_entities;
}

