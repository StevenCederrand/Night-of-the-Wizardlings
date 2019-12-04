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

    bool LoadProjectileSpell(std::string fileName);	// Load a file
    bool loadAOESpell(std::string fileName);	// Load a file
    void SaveProjectileSpell(std::string m_name, float m_ProjectileLowDmg, float m_ProjectileHighDmg,float m_ProjectileSpeed,
                    float m_ProjectileCooldown, float m_ProjectileRadius, float m_ProjectileLifetime, float m_ProjectileMaxBounces);

    void saveAOESpell(std::string name, int damage, int speed, int cooldown, int radius, int lifetime, int maxBounces);

    int getDamange();
    int getProjectileLowDmg();
    int getProjectileHighDmg();
	float getSpeed();
	float getCooldown();
	float getRadius();
	float getLifetime();
	float getMaxBounces();


	std::string m_fileName;
	std::string outputFilepath = "Exports/";

	// Temporary shared format to load data into
	SpellLoading::SpellHeader fileHeader;
	SpellLoading::Projectile m_projectile;
    SpellLoading::AOESpell m_AOESpell;

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
};
