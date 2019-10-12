#ifndef _NETWORK_SPELLS_H
#define _NETWORK_SPELLS_H
#include <Pch/Pch.h>
#include <Spells/SpellTypes.h>
#include <Spells/AttackSpell.h>
#include <Spells/EnhanceAttackSpell.h>

class NetworkSpells{

public:
	struct SpellEntity {
		SpellPacket spellData;
		bool IsAlive = false;
		Spell tempSpell; // Should be deleted when particles is done!!
	};

public:
	NetworkSpells();
	~NetworkSpells();
	void update(const float& dt);


private:
	std::vector<Spell> m_entities;
	AttackSpell* tempSpell;
	EnhanceAttackSpell* tempEnhanceAttackSpell;

};


#endif
