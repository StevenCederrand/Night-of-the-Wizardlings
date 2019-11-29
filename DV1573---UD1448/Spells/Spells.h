#ifndef	_SPELLS_h
#define _SPELLS_h
#include <Pch/Pch.h>
#include <Particles/Particles.h>
#include <Mesh/MeshFormat.h>
#include <Particles/ParticleBuffers.h>

struct AttackSpellBase
{
	Mesh* m_mesh;
	Material* m_material;

	float m_lowDamage;
	float m_highDamage;
	float m_speed;
	float m_coolDown;
	float m_radius;

	float m_lifeTime;
	float m_maxBounces;
	glm::vec4 m_attenAndRadius; //This is important for lights, xyz are attenuation values, while W is radius

	AttackSpellBase()
	{
		m_mesh = nullptr;
		m_material = nullptr;
		m_attenAndRadius = glm::vec4(1.0f, 0.14f, 0.07f, 12.0f);
		m_lowDamage = 0;
		m_highDamage = 0;
		m_speed = 0;
		m_radius = 0;
		m_coolDown = 0;

		m_lifeTime = 0;
		m_maxBounces = 0;
	}
	~AttackSpellBase()
	{
		if (m_mesh)
		{
			m_mesh->Destroy();
			delete m_mesh;
		}

		if (m_material)
		{
			delete m_material;
		}
	}
};


struct EnhanceAtkSpellBase
{
	Mesh* m_mesh;
	Material* m_material;

	float m_damage;
	float m_speed;
	float m_radius;

	float m_lifeTime;
	float m_maxBounces;
	glm::vec4 m_attenAndRadius; //This is important for lights, xyz are attenuation values, while W is radius
	EnhanceAtkSpellBase()
	{
		m_mesh = nullptr;
		m_material = nullptr;
		m_attenAndRadius = glm::vec4(1.0f, 0.14f, 0.07f, 12.0f);
		m_damage = 0;
		m_speed = 0;
		m_radius = 0;
		m_lifeTime = 0;
		m_maxBounces = 0;
	}
	~EnhanceAtkSpellBase()
	{
		if (m_mesh)
		{
			m_mesh->Destroy();
			delete m_mesh;
			m_mesh = nullptr;
		}

		if (m_material)
		{
			//m_material.Destory();
			delete m_material;
			m_material = nullptr;
		}
	}

};

struct ReflectSpellBase
{
	Mesh* m_mesh;
	Material* m_material;

	float m_radius;
	float m_coolDown;
	float m_lifeTime;
	glm::vec4 m_attenAndRadius; //This is important for lights, xyz are attenuation values, while W is radius
	ReflectSpellBase()
	{
		m_mesh = nullptr;
		m_material = nullptr;
		m_attenAndRadius = glm::vec4(1.0f, 0.14f, 0.07f, 12.0f);
		m_radius = 0;
		m_coolDown = 0;
		m_lifeTime = 0;
	}
	~ReflectSpellBase()
	{
		if (m_mesh)
		{
			m_mesh->Destroy();
			delete m_mesh;
			m_mesh = nullptr;
		}

		if (m_material)
		{
			//m_material.Destory();
			delete m_material;
			m_material = nullptr;
		}
	}

};


struct FlamestrikeSpellBase
{
	Mesh* m_mesh;
	Material* m_material;

	float m_damage;
	float m_speed;
	float m_coolDown;
	float m_radius;
	glm::vec4 m_attenAndRadius; //This is important for lights, xyz are attenuation values, while W is radius
	float m_lifeTime;
	float m_maxBounces;

	FlamestrikeSpellBase()
	{
		m_mesh = nullptr;
		m_material = nullptr;
		m_attenAndRadius = glm::vec4(1.0f, 0.14f, 0.07f, 12.0f);
		m_damage = 0;
		m_speed = 0;
		m_radius = 0;
		m_coolDown = 0;

		m_lifeTime = 0;
		m_maxBounces = 0;
	}
	~FlamestrikeSpellBase()
	{
		if (m_mesh)
		{
			m_mesh->Destroy();
			delete m_mesh;
			m_mesh = nullptr;
		}

		if (m_material)
		{
			//m_material.Destory();
			delete m_material;
			m_material = nullptr;
		}
	}

};

struct FireSpellBase
{
	Mesh* m_mesh;
	Material* m_material;
	
	std::vector<ParticleBuffers> m_partBuffers;
	std::vector<psBuffers> m_psBuffers;
	std::vector<PSinfo> m_PSinfo;
	std::vector<TextureInfo> m_txtInfo;
	std::vector<int> vertexCountDiff;
	std::vector<float> emissionDiff;

	float m_damage;
	float m_speed;
	float m_coolDown;
	float m_radius;
	glm::vec4 m_attenAndRadius; //This is important for lights, xyz are attenuation values, while W is radius
	float m_lifeTime;
	float m_maxBounces;

	FireSpellBase()
	{
		m_mesh = nullptr;
		m_material = nullptr;
		m_attenAndRadius = glm::vec4(1.0f, 0.14f, 0.07f, 12.0f);
		m_damage = 0;
		m_speed = 0;
		m_radius = 0;
		m_coolDown = 0;

		m_lifeTime = 0;
		m_maxBounces = 0;
	}
	~FireSpellBase()
	{
		if (m_mesh)
		{
			m_mesh->Destroy();
			delete m_mesh;
			m_mesh = nullptr;
		}

		if (m_material)
		{
			//m_material.Destory();
			delete m_material;
			m_material = nullptr;
		}
	}

};

/*
	7	1.0	0.7	1.8
	13	1.0	0.35	0.44
	20	1.0	0.22	0.20
	32	1.0	0.14	0.07
	50	1.0	0.09	0.032
	65	1.0	0.07	0.017
	100	1.0	0.045	0.0075
	160	1.0	0.027	0.0028
	200	1.0	0.022	0.0019
	325	1.0	0.014	0.0007
	600	1.0	0.007	0.0002
	3250	1.0	0.0014	0.000007
*/


#endif
