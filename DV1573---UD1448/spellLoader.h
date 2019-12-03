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
	void SaveSpell(std::string m_name, float m_ProjectileLowDmg, float m_ProjectileHighDmg,float m_ProjectileSpeed, float m_ProjectileCooldown, float m_ProjectileRadius, float m_ProjectileLifetime, float m_ProjectileMaxBounces);
    int getProjectileLowDmg();
    int getProjectileHighDmg();
	float getProjectileSpeed();
	float getProjectileCooldown();
	float getProjectileRadius();
	float getProjectileLifetime();
	float getProjectileMaxBounces();


	std::string m_fileName;
	std::string outputFilepath = "Exports/";

	// Temporary shared format to load data into
	SpellLoading::SpellHeader fileHeader;
	SpellLoading::Projectile m_projectile;

	int m_nrOfSpells = 1;

private:


	//-----Projectile Data-----//
	std::string m_name;
    float m_ProjectileLowDmg;
    float m_ProjectileHighDmg;
	float m_ProjectileSpeed;								
	float m_ProjectileCooldown;
	float m_ProjectileRadius;
	float m_ProjectileLifetime;
	float m_ProjectileMaxBounces;
};
