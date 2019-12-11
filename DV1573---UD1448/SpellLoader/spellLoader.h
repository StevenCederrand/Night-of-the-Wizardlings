#pragma once
#include <Pch/Pch.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <SpellLoader/loaderFormat.h>
//#include <States/SpellCreatorState.h>

class SpellLoader
{
public:
	SpellLoader(std::string fileName);
	SpellLoader();
	~SpellLoader();


    bool LoadProjectileSpell(std::string fileName);	// Load a file
    bool loadAOESpell(std::string fileName);	// Load a file
    void SaveProjectileSpell(std::string name, SpellLoading::Projectile projectileInfo, SpellLoading::SpellEvents spellEvent, PSinfo psInfo, TextureInfo txtInfo);

    void saveAOESpell(std::string name, int damage, int speed, int cooldown, int radius, int lifetime, int maxBounces);

	std::string m_fileName;
	std::string outputFilepath = "Exports/";

	// Temporary shared format to load data into
	SpellLoading::SpellHeader fileHeader;
	SpellLoading::Projectile m_projectile;
    SpellLoading::AOESpell m_AOESpell;
    SpellLoading::SpellEvents m_spellEvents;
	PSinfo m_psInfo;
	TextureInfo m_txtInfo;

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


	//-----Particle Data----//
	glm::vec3 m_direction;
	glm::vec3 m_color;
	glm::vec3 m_blendColor;

	int m_maxParticles;
	bool m_cont;
	bool m_omnious;
	int m_seed;
	int m_fade;
	int m_scaleDirection;
	float m_glow;
	int m_swirl;
	bool m_randomSpawn;

	float m_width;
	float m_heigth;
	float m_lifetimeP;
	float m_emission;
	float m_spread;
	float m_force;
	float m_drag;
	float m_gravity;

	char m_textureName[NAME_SIZE];



};
