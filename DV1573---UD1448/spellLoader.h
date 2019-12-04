#pragma once
#include <Pch/Pch.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include "loaderFormat.h"


class SpellLoader
{
public:
	SpellLoader(std::string fileName);
	SpellLoader();
	~SpellLoader();

	bool LoadSpell(std::string fileName);	// Load a file

    void SaveProjectileSpell(std::string m_name, float m_ProjectileLowDmg, float m_ProjectileHighDmg,float m_ProjectileSpeed,
           float m_ProjectileCooldown, float m_ProjectileRadius, float m_ProjectileLifetime, float m_ProjectileMaxBounces,
           int m_nrOfEvents, int m_firstEvent, int m_secondEvent, int m_thirdEvent, int m_fourthEvent, int m_fifthEvent);

    void saveAOESpell(std::string name, int damage, int speed, int cooldown, int radius, int lifetime, int maxBounces);

	std::string m_fileName;
	std::string outputFilepath = "Exports/";

	// Temporary shared format to load data into
	SpellLoading::SpellHeader fileHeader;
	SpellLoading::Projectile m_projectile;
    SpellLoading::AOESpell m_AOESpell;
    SpellLoading::SpellEvents m_spellEvents;

	int m_nrOfSpells = 1;

private:


	//-----Projectile Data-----//
	std::string m_name;
    float m_damage;
    float m_lowDmg;
    float m_highDmg;
	float m_speed;								
	float m_cooldown;
	float m_radius;
	float m_lifetime;
	float m_maxBounces;

    //-----Spell Event Data-----//
    float m_nrOfEvents;
    float m_firstEvent;
    float m_secondEvent;
    float m_thirdEvent;
    float m_fourthEvent;
    float m_fifthEvent;
};
