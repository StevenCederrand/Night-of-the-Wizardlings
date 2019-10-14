#pragma once
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Spells/AttackSpell.h>
#include <Spells/EnhanceAttackSpell.h>
#include <System/BulletPhysics.h>

enum TYPE { NORMALATTACK, ENHANCEATTACK };

class SpellHandler
{
public:
	SpellHandler(glm::vec3 playerPosition, glm::vec3 directionVector, BulletPhysics* bp);
	~SpellHandler();
	void createSpell(float deltaTime, glm::vec3 spellPos, glm::vec3 directionVector, TYPE type);
	void spellUpdate(float deltaTime);
	void spellCooldown(float deltaTime);
	void renderSpell();

private:
	std::vector<AttackSpell> normalSpell;
	AttackSpell* tempSpell;
	std::vector<EnhanceAttackSpell> enhanceAttackSpell;
	EnhanceAttackSpell* tempEnhanceAttackSpell;

	glm::vec3 directionVector;
	glm::vec3 spellPos;

	void spellCollisionCheck();
	bool specificSpellCollision(glm::vec3 spellPos, glm::vec3 playerPos, std::vector<glm::vec3>& axis);
	glm::vec3 OBBclosestPoint(glm::vec3 &spherePos, std::vector<glm::vec3> &axis, glm::vec3 &playerPos);
	BulletPhysics* m_bp;

	std::vector<btRigidBody*> m_BulletNormalSpell;
	std::vector<btRigidBody*> m_BulletEnhanceAttackSpell;


};