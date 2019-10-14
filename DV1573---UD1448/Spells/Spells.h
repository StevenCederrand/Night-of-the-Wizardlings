#ifndef	_SPELLS_h
#define _SPELLS_h
#include <Pch/Pch.h>

struct AttackSpellBase
{
	Mesh* m_mesh;
	Material* m_material;

	float m_damage;
	float m_speed;
	float m_coolDown;
	
	float m_lifeTime;
	float m_maxBounces;

	AttackSpellBase()
	{
		m_mesh = nullptr;
		m_material = nullptr;
	}
	~AttackSpellBase()
	{
		m_mesh->Destroy();
		delete m_mesh;
		m_mesh = nullptr;
		//m_material.Destory();
		delete m_material;
		m_material = nullptr;
	}
};


#endif
