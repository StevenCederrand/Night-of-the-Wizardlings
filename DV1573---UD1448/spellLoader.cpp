#include <Pch/Pch.h>
#include "spellLoader.h"

SpellLoader::SpellLoader(std::string fileName)
{
	m_fileName = fileName;

	LoadSpell(fileName);
}

SpellLoader::SpellLoader()
{
}

SpellLoader::~SpellLoader()
{
}

bool SpellLoader::LoadSpell(std::string file)
{
	//unload();
	//m_fileName = file;
	//std::ifstream binFile(outputFilepath + m_fileName + ".spell", std::ios::binary);

    std::ifstream binFile(file, std::ios::binary);
	if (!binFile)
	{
		return false;
	}
	else
	{
		// Read the header
		binFile.read((char*)&fileHeader, sizeof(SpellLoading::SpellHeader));
		m_nrOfSpells = fileHeader.spellCount;

		// Read the Projectile struct
		binFile.read((char*)&m_projectile, sizeof(SpellLoading::Projectile));
		
		// Fill Projectile data
		m_name                 = m_projectile.name;
        m_ProjectileLowDmg     = m_projectile.m_lowDamage;
        m_ProjectileHighDmg    = m_projectile.m_highDamage;
		m_ProjectileSpeed      = m_projectile.m_speed;
		m_ProjectileCooldown   = m_projectile.m_coolDown;
		m_ProjectileRadius     = m_projectile.m_radius;
		m_ProjectileLifetime   = m_projectile.m_lifeTime;
		m_ProjectileMaxBounces = m_projectile.m_maxBounces;
	}
	binFile.close();

	return true;
}

void SpellLoader::SaveSpell(std::string m_name, float m_ProjectileLowDmg, float m_ProjectileHighDmg,float m_ProjectileSpeed, float m_ProjectileCooldown, float m_ProjectileRadius, float m_ProjectileLifetime, float m_ProjectileMaxBounces)
{
	fileHeader.spellCount = m_nrOfSpells;
	
	//-----Assign all the Projectile spell data-----//
    m_projectile.m_lowDamage = m_ProjectileLowDmg;
    m_projectile.m_highDamage = m_ProjectileHighDmg;
	m_projectile.m_speed = m_ProjectileSpeed;
	m_projectile.m_coolDown = m_ProjectileCooldown;
	m_projectile.m_radius = m_ProjectileRadius;
	m_projectile.m_lifeTime = m_ProjectileLifetime;
	m_projectile.m_maxBounces = m_ProjectileMaxBounces;

	//-----Convert a string to char and assign the char to the struct-----//
	int stringLength = m_name.length();
	char tempCharName[NAME_SIZE];
	m_name.copy(tempCharName, stringLength);
	tempCharName[stringLength] = '\0';
	for (int i = 0; i < stringLength + 1; i++)
	{
		m_projectile.name[i] = tempCharName[i];
	}

	//-----===== Write ASCII file =====-----//
	std::ofstream asciiFile;
	asciiFile.open(outputFilepath + m_name + ".txt");
	asciiFile << std::fixed << std::setprecision(10);

	std::cout << "writing ascii to my testfile //Daniel" << std::endl;
	asciiFile << "// File Header ----------" << std::endl;
	asciiFile << fileHeader.spellCount << std::endl;

	asciiFile << "// Spells ----------" << std::endl;
	for (int i = 0; i < stringLength; i++)
	{
		asciiFile << m_projectile.name[i];
	}
	asciiFile << '\0';
	asciiFile << std::endl;

    asciiFile << m_projectile.m_lowDamage << std::endl;
    asciiFile << m_projectile.m_highDamage << std::endl;
	asciiFile << m_projectile.m_speed << std::endl;
	asciiFile << m_projectile.m_coolDown << std::endl;
	asciiFile << m_projectile.m_radius << std::endl;
	asciiFile << m_projectile.m_lifeTime << std::endl;
	asciiFile << m_projectile.m_maxBounces << std::endl;

	asciiFile.close();
	std::cout << "ascii file done!" << std::endl;

	//-----===== Write BINARY file =====-----//
	std::cout << "writing binary to my testfile //Daniel" << std::endl;
	std::ofstream binaryFile(outputFilepath + m_name + ".spell", std::ofstream::binary);	// This is where out the filepath should be added comming in from the CMD

	binaryFile.write((char*)& fileHeader, sizeof(SpellLoading::SpellHeader));
	binaryFile.write((char*)& m_projectile, sizeof(SpellLoading::Projectile));

	binaryFile.close();
	std::cout << "binary file done!" << std::endl;

}

int SpellLoader::getProjectileLowDmg()
{
	return m_ProjectileLowDmg;
}

int SpellLoader::getProjectileHighDmg()
{
	return m_ProjectileHighDmg;
}

float SpellLoader::getProjectileSpeed()
{
	return m_ProjectileSpeed;
}

float SpellLoader::getProjectileCooldown()
{
	return m_ProjectileCooldown;
}

float SpellLoader::getProjectileRadius()
{
	return m_ProjectileRadius;
}

float SpellLoader::getProjectileLifetime()
{
	return m_ProjectileLifetime;
}

float SpellLoader::getProjectileMaxBounces()
{
	return m_ProjectileMaxBounces;
}
