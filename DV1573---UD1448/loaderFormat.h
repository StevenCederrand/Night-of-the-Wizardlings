#pragma once
#define NAME_SIZE 256

// Format that both the exporter and importer share
namespace SpellLoading
{
	struct SpellHeader
	{
		int spellCount;
	};

	struct Projectile
	{
		char name[NAME_SIZE];
        float m_lowDamage;
        float m_highDamage;
		float m_speed;
		float m_coolDown;
		float m_radius;
		float m_lifeTime;
		float m_maxBounces;
	};

    struct AOESpell
    {
        char name[NAME_SIZE];
        int m_damage;
        int m_speed;
        int m_coolDown;
        int m_radius;
        int m_lifeTime;
        int m_maxBounces;
    };

    struct SpellEvents
    {
        int m_nrOfEvents;
        int m_firstEvent;
        int m_secondEvent;
        int m_thirdEvent;
        int m_fourthEvent;
        int m_fifthEvent;
    };
}


