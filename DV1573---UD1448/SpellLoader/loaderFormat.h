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
		int maxBounces;
		glm::vec3 color;
	};

    struct AOESpell
    {
        char name[NAME_SIZE];
		float damage;
		float speed;
		float coolDown;
		float radius;
		int lifeTime;
        int maxBounces;
		glm::vec3 color;

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


