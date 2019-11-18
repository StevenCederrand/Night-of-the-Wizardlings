#ifndef _NETWORK_SPELLS_H
#define _NETWORK_SPELLS_H
#include <Pch/Pch.h>
#include <Spells/SpellTypes.h>


class NetworkSpells{

public:

	struct SpellEntity {
		SpellPacket spellData;
		GameObject* gameobject = nullptr;
		NetGlobals::THREAD_FLAG flag = NetGlobals::THREAD_FLAG::None;		
	};

public:
	NetworkSpells();
	~NetworkSpells();
	void update(const float& dt);
	void cleanUp();
	std::vector<SpellEntity>& getSpellEntitiesREF();

private:
	friend class Client;
	std::vector<SpellEntity> m_entities;
	float m_lerpSpeed = 15.0f;
};


#endif
