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

	float m_damage;
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
		m_attenAndRadius = glm::vec4(1.0, 0.14, 0.07, 12.0);
		m_damage = 0;
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
		m_attenAndRadius = glm::vec4(1.0, 0.14, 0.07, 12.0);
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
		m_attenAndRadius = glm::vec4(1.0, 0.14, 0.07, 12.0);
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
		m_attenAndRadius = glm::vec4(1.0, 0.14, 0.07, 12.0);
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
		m_attenAndRadius = glm::vec4(1.0, 0.14, 0.07, 12.0);
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

#endif
