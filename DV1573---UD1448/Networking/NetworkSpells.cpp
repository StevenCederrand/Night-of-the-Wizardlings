#include <Pch/Pch.h>
#include "NetworkSpells.h"
#include "Client.h"

NetworkSpells::NetworkSpells()
{
	tempSpell = new AttackSpell("Spell", glm::vec3(0.0f), glm::vec3(0.0f), 50, 2, "TestSphere.mesh", 0);
	tempEnhanceAttackSpell = new EnhanceAttackSpell("EnhanceSpell", glm::vec3(0.0f), glm::vec3(0.0f), 10, 4, "TestCube.mesh", 0, 3);
}



NetworkSpells::~NetworkSpells()
{

}

void NetworkSpells::update(const float& dt)
{
	if (Client::getInstance()->isConnectedToSever()) {

		auto& map = Client::getInstance()->getNetworkSpells();

		for (auto const& item : map)
		{
			auto& spellList = item.second;

			for (size_t i = 0; i < spellList.size(); i++) {

				auto& packet = spellList[i];




			}

		}
	}
}

