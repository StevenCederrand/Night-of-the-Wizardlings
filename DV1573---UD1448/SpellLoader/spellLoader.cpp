#include <Pch/Pch.h>
#include "spellLoader.h"

SpellLoader::SpellLoader(std::string fileName)
{
	m_fileName = fileName;

    loadAOESpell(fileName);
    LoadProjectileSpell(fileName);
}

SpellLoader::SpellLoader()
{
}

SpellLoader::~SpellLoader()
{
}

bool SpellLoader::LoadProjectileSpell(std::string file)
{
	//unload();
	m_fileName = file;

    //std::ifstream binFile(outputFilepath + m_fileName + ".spell", std::ios::binary);
    std::ifstream binFile(outputFilepath + m_fileName, std::ios::binary);
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
        m_lowDmg             = m_projectile.lowDamage;
        m_highDmg            = m_projectile.highDamage;
		m_speed              = m_projectile.speed;
		m_cooldown           = m_projectile.coolDown;
		m_radius             = m_projectile.radius;
		m_lifetime           = m_projectile.lifeTime;
		m_maxBounces         = m_projectile.maxBounces;

        // Read the Spell Events struct
        binFile.read((char*)& m_spellEvents, sizeof(SpellLoading::SpellEvents));

        // Fill Spell Event data
        m_nrOfEvents         = m_spellEvents.nrOfEvents;
        m_firstEvent         = m_spellEvents.firstEvent;
        m_secondEvent        = m_spellEvents.secondEvent;
        m_thirdEvent         = m_spellEvents.thirdEvent;
        m_fourthEvent        = m_spellEvents.fourthEvent;
        m_fifthEvent         = m_spellEvents.fifthEvent;

		binFile.read((char*)& m_psInfo, sizeof(PSinfo));
		m_width = m_psInfo.width;
		m_heigth = m_psInfo.heigth;
		m_lifetimeP = m_psInfo.lifetime;
		m_maxParticles = m_psInfo.maxParticles;
		m_emission = m_psInfo.emission;
		m_force = m_psInfo.force;
		m_drag = m_psInfo.drag;
		m_gravity = m_psInfo.gravity;
		m_seed = 0; //HARDCODED
		m_cont = m_psInfo.cont;
		m_omnious = m_psInfo.omnious;
		m_randomSpawn = m_psInfo.randomSpawn;
		m_spread = m_psInfo.spread;
		m_glow = m_psInfo.glow;
		m_scaleDirection = m_psInfo.scaleDirection;
		m_swirl = m_psInfo.swirl;
		m_fade = m_psInfo.fade;
		m_color = m_psInfo.color;
		m_blendColor = m_psInfo.blendColor;
		m_direction = m_psInfo.direction;

		binFile.read((char*)& m_textureName, sizeof(m_textureName));
		m_txtInfo.name = m_textureName;
		//Fill particle data
		//m_textureName = m_txtInfo.name;
	}
	binFile.close();

	return true;
}

bool SpellLoader::loadAOESpell(std::string fileName)
{
    //unload();
    m_fileName = fileName;

    std::ifstream binFile(outputFilepath + m_fileName, std::ios::binary);
    if (!binFile)
    {
        return false;
    }
    else
    {
        // Read the header
        binFile.read((char*)& fileHeader, sizeof(SpellLoading::SpellHeader));
        m_nrOfSpells = fileHeader.spellCount;

        // Read the Projectile struct
        binFile.read((char*)&m_AOESpell, sizeof(SpellLoading::AOESpell));

        // Fill Projectile data
        m_name			= m_AOESpell.name;
        m_damage		= m_AOESpell.damage;
        m_speed			= m_AOESpell.speed;
        m_cooldown		= m_AOESpell.coolDown;
        m_radius		= m_AOESpell.radius;
        m_lifetime		= m_AOESpell.lifeTime;
        m_maxBounces	= m_AOESpell.maxBounces;
    }
    binFile.close();

    return true;
}

void SpellLoader::SaveProjectileSpell(std::string name, SpellLoading::Projectile projectileInfo, SpellLoading::SpellEvents spellEvent, PSinfo psInfo, TextureInfo txtInfo)
{
	fileHeader.spellCount = m_nrOfSpells;
	
	//-----Assign all the Projectile spell data-----//
    m_projectile.lowDamage  = projectileInfo.lowDamage;
	m_projectile.highDamage	= projectileInfo.highDamage;
	m_projectile.speed		= projectileInfo.speed;
	m_projectile.coolDown	= projectileInfo.coolDown;
	m_projectile.radius		= projectileInfo.radius;
	m_projectile.lifeTime	= projectileInfo.lifeTime;
	m_projectile.maxBounces	= projectileInfo.maxBounces;

    //-----Assign all the Spell event data-----//
    m_spellEvents.nrOfEvents  = spellEvent.nrOfEvents;
    m_spellEvents.firstEvent  = spellEvent.firstEvent;
    m_spellEvents.secondEvent = spellEvent.secondEvent;
    m_spellEvents.thirdEvent  = spellEvent.thirdEvent;
    m_spellEvents.fourthEvent = spellEvent.fourthEvent;
    m_spellEvents.fifthEvent  = spellEvent.fifthEvent;

	//-----Assign all the particle data-----//
	m_txtInfo.name			= txtInfo.name;
	m_psInfo.width			= psInfo.width;
	m_psInfo.heigth			= psInfo.heigth;
	m_psInfo.lifetime		= psInfo.lifetime;
	m_psInfo.maxParticles	= psInfo.maxParticles;
	m_psInfo.emission		= psInfo.emission;
	m_psInfo.force			= psInfo.force;
	m_psInfo.drag			= psInfo.drag;
	m_psInfo.gravity		= psInfo.gravity;
	m_psInfo.spread			= psInfo.spread;
	m_psInfo.glow			= psInfo.glow;
	m_psInfo.scaleDirection = psInfo.scaleDirection;
	m_psInfo.swirl			= psInfo.swirl;
	m_psInfo.fade			= psInfo.fade;
	m_psInfo.randomSpawn	= psInfo.randomSpawn;
	m_psInfo.cont			= psInfo.cont;
	m_psInfo.omnious		= psInfo.omnious;
	m_psInfo.color			= psInfo.color;
	m_psInfo.blendColor		= psInfo.blendColor;

	//-----Convert a string to char and assign the char to the struct-----//
	int stringLength = name.length();
	char tempCharName[NAME_SIZE];
	name.copy(tempCharName, stringLength);
	tempCharName[stringLength] = '\0';
	for (int i = 0; i < stringLength + 1; i++)
	{
		m_projectile.name[i] = tempCharName[i];
	}

	stringLength = m_txtInfo.name.length();
	tempCharName[NAME_SIZE];
	m_txtInfo.name.copy(tempCharName, stringLength);
	tempCharName[stringLength] = '\0';
	for (int i = 0; i < stringLength + 1; i++)
	{
		m_textureName[i] = tempCharName[i];
	}

	//-----===== Write ASCII file =====-----//
	std::ofstream asciiFile;
	asciiFile.open(outputFilepath + name + ".txt");
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

    asciiFile << m_projectile.lowDamage << std::endl;
    asciiFile << m_projectile.highDamage << std::endl;
	asciiFile << m_projectile.speed << std::endl;
	asciiFile << m_projectile.coolDown << std::endl;
	asciiFile << m_projectile.radius << std::endl;
	asciiFile << m_projectile.lifeTime << std::endl;
	asciiFile << m_projectile.maxBounces << std::endl;

    asciiFile << "// Events ----------" << std::endl;
    asciiFile << m_spellEvents.nrOfEvents << std::endl;
    asciiFile << m_spellEvents.firstEvent << std::endl;
    asciiFile << m_spellEvents.secondEvent << std::endl;
    asciiFile << m_spellEvents.thirdEvent << std::endl;
    asciiFile << m_spellEvents.fourthEvent << std::endl;
    asciiFile << m_spellEvents.fifthEvent << std::endl;

	asciiFile << "// particles ----------" << std::endl;
	asciiFile << m_txtInfo.name << std::endl;
	asciiFile << m_psInfo.width << std::endl;
	asciiFile << m_psInfo.heigth << std::endl;
	asciiFile << m_psInfo.lifetime << std::endl;
	asciiFile << m_psInfo.maxParticles << std::endl;
	asciiFile << m_psInfo.emission << std::endl;
	asciiFile << m_psInfo.force << std::endl;
	asciiFile << m_psInfo.drag << std::endl;
	asciiFile << m_psInfo.gravity << std::endl;
	asciiFile << m_psInfo.spread << std::endl;
	asciiFile << m_psInfo.glow << std::endl;
	asciiFile << m_psInfo.scaleDirection << std::endl;
	asciiFile << m_psInfo.swirl << std::endl;
	asciiFile << m_psInfo.fade << std::endl;
	asciiFile << m_psInfo.randomSpawn << std::endl;
	asciiFile << m_psInfo.cont << std::endl;
	asciiFile << m_psInfo.omnious << std::endl;
	asciiFile << m_psInfo.color.x << " " << m_psInfo.color.y << " " << m_psInfo.color.z << std::endl;
	asciiFile << m_psInfo.blendColor.x << " " << m_psInfo.blendColor.y << " " << m_psInfo.blendColor.z << std::endl;

	asciiFile.close();
	std::cout << "ascii file done!" << std::endl;

	//-----===== Write BINARY file =====-----//
	std::cout << "writing binary to my testfile //Daniel" << std::endl;
	std::ofstream binaryFile(outputFilepath + name + ".spell", std::ofstream::binary);	// This is where out the filepath should be added comming in from the CMD

	binaryFile.write((char*)& fileHeader, sizeof(SpellLoading::SpellHeader));
    binaryFile.write((char*)& m_projectile, sizeof(SpellLoading::Projectile));
	binaryFile.write((char*)& m_spellEvents, sizeof(SpellLoading::SpellEvents));
	binaryFile.write((char*)& m_psInfo, sizeof(PSinfo));
	binaryFile.write((char*)& m_textureName, sizeof(m_textureName));

	binaryFile.close();
	std::cout << "binary file done!" << std::endl;

}

void SpellLoader::saveAOESpell(std::string name, int damage, int speed, int cooldown, int radius, int lifetime, int maxBounces)
{
    fileHeader.spellCount = m_nrOfSpells;

    //-----Assign all the Projectile spell data-----//

    m_AOESpell.damage = damage;
    m_AOESpell.speed = speed;
    m_AOESpell.coolDown = cooldown;
    m_AOESpell.radius = radius;
    m_AOESpell.lifeTime = lifetime;
    m_AOESpell.maxBounces = maxBounces;

    //-----Convert a string to char and assign the char to the struct-----//
    int stringLength = name.length();
    char tempCharName[NAME_SIZE];
    name.copy(tempCharName, stringLength);
    tempCharName[stringLength] = '\0';
    for (int i = 0; i < stringLength + 1; i++)
    {
        m_AOESpell.name[i] = tempCharName[i];
    }

    //-----===== Write ASCII file =====-----//
    std::ofstream asciiFile;
    asciiFile.open(outputFilepath + name + ".txt");
    asciiFile << std::fixed << std::setprecision(10);

    std::cout << "writing ascii to my testfile //Daniel" << std::endl;
    asciiFile << "// File Header ----------" << std::endl;
    asciiFile << fileHeader.spellCount << std::endl;

    asciiFile << "// Spells ----------" << std::endl;
    for (int i = 0; i < stringLength; i++)
    {
        asciiFile << m_AOESpell.name[i];
    }
    asciiFile << '\0';
    asciiFile << std::endl;

    asciiFile << m_AOESpell.damage << std::endl;
    asciiFile << m_AOESpell.speed << std::endl;
    asciiFile << m_AOESpell.coolDown << std::endl;
    asciiFile << m_AOESpell.radius << std::endl;
    asciiFile << m_AOESpell.lifeTime << std::endl;
    asciiFile << m_AOESpell.maxBounces << std::endl;

    asciiFile.close();
    std::cout << "ascii file done!" << std::endl;

    //-----===== Write BINARY file =====-----//
    std::cout << "writing binary to my testfile //Daniel" << std::endl;
    std::ofstream binaryFile(outputFilepath + name + ".spell", std::ofstream::binary);	// This is where out the filepath should be added comming in from the CMD

    binaryFile.write((char*)& fileHeader, sizeof(SpellLoading::SpellHeader));
    binaryFile.write((char*)& m_AOESpell, sizeof(SpellLoading::AOESpell));

    binaryFile.close();
    std::cout << "binary file done!" << std::endl;
}

