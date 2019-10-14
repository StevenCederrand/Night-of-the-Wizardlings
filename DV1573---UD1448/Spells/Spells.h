#ifndef _SPELLS_h
#define _SPELLS_h

//Normal Attack
struct NormalAttack {
	Mesh* m_mesh;
	Material* m_material;
	float m_speed;
	float m_bounces;
	float m_coolDown;
};
#endif