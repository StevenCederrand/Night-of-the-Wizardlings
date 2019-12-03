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
}


