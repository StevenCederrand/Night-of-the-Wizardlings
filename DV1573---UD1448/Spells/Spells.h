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

	AttackSpellBase()
	{
		m_mesh = nullptr;
		m_material = nullptr;

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

	EnhanceAtkSpellBase()
	{
		m_mesh = nullptr;
		m_material = nullptr;

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

	ReflectSpellBase()
	{
		m_mesh = nullptr;
		m_material = nullptr;

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

	float m_lifeTime;
	float m_maxBounces;

	FlamestrikeSpellBase()
	{
		m_mesh = nullptr;
		m_material = nullptr;

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

	float m_lifeTime;
	float m_maxBounces;

	FireSpellBase()
	{
		m_mesh = nullptr;
		m_material = nullptr;

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
