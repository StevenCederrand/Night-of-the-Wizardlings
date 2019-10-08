#ifndef _NETWORK_SPELLS_H
#define _NETWORK_SPELLS_H
#include <Pch/Pch.h>
#include <Spells/SpellTypes.h>
#include <Spells/Spell.h>

#define MAX_SPELLS_IN_WORLD 64

class NetworkSpells{

public:
	struct SpellEntity {
		SpellPacket spellData;
		bool IsAlive = false;
		Spell* tempSpell; // Should be deleted when particles is done!!
	};

public:
	NetworkSpells();
	~NetworkSpells();
	//void createSpell(SPELL_TYPE type, )

private:
	std::array<SpellEntity, MAX_SPELLS_IN_WORLD> m_spells;
	
};


#endif
