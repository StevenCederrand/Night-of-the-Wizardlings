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
        float lowDamage;
        float highDamage;
		float speed;
		float coolDown;
		float radius;
		float lifeTime;
		float maxBounces;
	};

    struct AOESpell
    {
        char name[NAME_SIZE];
        int damage;
        int speed;
        int coolDown;
        int radius;
        int lifeTime;
        int maxBounces;
    };

    struct SpellEvents
    {
        int nrOfEvents;
        int firstEvent;
        int secondEvent;
        int thirdEvent;
        int fourthEvent;
        int fifthEvent;
    };
}


