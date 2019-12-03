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
	m_fileName = file;

	std::ifstream binFile(outputFilepath + m_fileName + ".spell", std::ios::binary);
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
		m_name               = m_projectile.name;
        m_lowDmg             = m_projectile.m_lowDamage;
        m_highDmg            = m_projectile.m_highDamage;
		m_speed              = m_projectile.m_speed;
		m_cooldown           = m_projectile.m_coolDown;
		m_radius             = m_projectile.m_radius;
		m_lifetime           = m_projectile.m_lifeTime;
		m_maxBounces         = m_projectile.m_maxBounces;
	}
	binFile.close();

	return true;
}

void SpellLoader::SaveProjectileSpell(std::string m_name, float m_ProjectileLowDmg, float m_ProjectileHighDmg,float m_ProjectileSpeed, float m_ProjectileCooldown, float m_ProjectileRadius, float m_ProjectileLifetime, float m_ProjectileMaxBounces)
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

void SpellLoader::saveAOESpell(std::string name, int damage, int speed, int cooldown, int radius, int lifetime, int maxBounces)
{
    fileHeader.spellCount = m_nrOfSpells;

    //-----Assign all the Projectile spell data-----//

    m_AOESpell.m_damage = damage;
    m_AOESpell.m_speed = speed;
    m_AOESpell.m_coolDown = cooldown;
    m_AOESpell.m_radius = radius;
    m_AOESpell.m_lifeTime = lifetime;
    m_AOESpell.m_maxBounces = maxBounces;

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

    asciiFile << m_AOESpell.m_damage << std::endl;
    asciiFile << m_AOESpell.m_speed << std::endl;
    asciiFile << m_AOESpell.m_coolDown << std::endl;
    asciiFile << m_AOESpell.m_radius << std::endl;
    asciiFile << m_AOESpell.m_lifeTime << std::endl;
    asciiFile << m_AOESpell.m_maxBounces << std::endl;

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

int SpellLoader::getDamange()
{
    return m_damage;
}

int SpellLoader::getProjectileLowDmg()
{
	return m_lowDmg;
}

int SpellLoader::getProjectileHighDmg()
{
	return m_highDmg;
}

float SpellLoader::getSpeed()
{
	return m_speed;
}

float SpellLoader::getCooldown()
{
	return m_cooldown;
}

float SpellLoader::getRadius()
{
	return m_radius;
}

float SpellLoader::getLifetime()
{
	return m_lifetime;
}

float SpellLoader::getMaxBounces()
{
	return m_maxBounces;
}
