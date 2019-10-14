#include <Pch/Pch.h>
#include "NetworkSpells.h"
#include "Client.h"

NetworkSpells::NetworkSpells()
{
	/*tempSpell = new AttackSpell("Spell", glm::vec3(0.0f), glm::vec3(0.0f), 50, 2, "TestSphere.mesh", 0);
	tempEnhanceAttackSpell = new EnhanceAttackSpell("EnhanceSpell", glm::vec3(0.0f), glm::vec3(0.0f), 10, 4, "TestCube.mesh", 0, 3);*/
}



NetworkSpells::~NetworkSpells()
{
	for (size_t i = 0; i < m_entities.size(); i++) {
		if (m_entities[i].tempObject != nullptr)
			delete m_entities[i].tempObject;
	}

}

void NetworkSpells::update(const float& dt)
{
	if (Client::getInstance()->isConnectedToSever()) {
		
		std::lock_guard<std::mutex> lockGuard(NetGlobals::gameSyncMutex);
		//logTrace("Network spells got the mutex!");
		for (size_t i = 0; i < m_entities.size(); i++) {
			//logTrace("Getting entity, networkSpells");
			SpellEntity& e = m_entities[i];
			//logTrace("Done getting entity, networkSpells");
			if (e.flag == FLAG::ADD) {
				if (e.tempObject == nullptr) {
					
					e.tempObject = new WorldObject();
					
					if (e.spellData.SpellType == SPELL_TYPE::NORMALATTACK || e.spellData.SpellType == SPELL_TYPE::UNKNOWN) {
						e.tempObject->loadMesh("TestSphere.mesh");
					}
					else if (e.spellData.SpellType == SPELL_TYPE::ENHANCEATTACK) {
						e.tempObject->loadMesh("TestCube.mesh");
					}
				
					e.tempObject->setWorldPosition(e.spellData.Position);
					Renderer::getInstance()->submit(e.tempObject, DYNAMIC);
					e.flag == FLAG::NONE;

				}
			}
			else if (e.flag == FLAG::REMOVE)
			{
				Renderer::getInstance()->removeDynamic(e.tempObject);
				delete e.tempObject;
				m_entities.erase(m_entities.begin() + i);
				i--;
				continue;
			}

			GameObject* g = e.tempObject;

			if (g != nullptr) {

				//logTrace("Game object position before: {0}, {1}, {2}", g->getTransform().position.x, g->getTransform().position.y, g->getTransform().position.z);
				//logTrace("Spell data position: {0}, {1}, {2}", e.spellData.Position.x, e.spellData.Position.y, e.spellData.Position.z);
				glm::vec3 pos = CustomLerp(g->getTransform().position, e.spellData.Position, m_lerpSpeed * dt);
				//logTrace("Game object position after: {0}, {1}, {2}", pos.x, pos.y, pos.z);
				g->setWorldPosition(pos);
				//g->setTransform(pos, glm::quat(p->data.rotation));

			}

			//logTrace("Done with update, networkSpells");
		}
	}
}

std::vector<NetworkSpells::SpellEntity>& NetworkSpells::getSpellEntitiesREF()
{
	return m_entities;
}

