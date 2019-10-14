#ifndef _NETWORK_SPELLS_H
#define _NETWORK_SPELLS_H
#include <Pch/Pch.h>
#include <Spells/SpellTypes.h>


class NetworkSpells{

public:
	enum FLAG {
		REMOVE,
		ADD,
		NONE
	};


	struct SpellEntity {
		SpellPacket spellData;
		GameObject* tempObject; // Should be deleted when particles is done!!
		FLAG flag = FLAG::NONE;
	};

public:
	NetworkSpells();
	~NetworkSpells();
	void update(const float& dt);
	std::vector<SpellEntity>& getSpellEntitiesREF();

private:
	friend class Client;
	std::mutex m_mutex;
	std::vector<SpellEntity> m_entities;
	float m_lerpSpeed = 15.0f;
};


#endif
