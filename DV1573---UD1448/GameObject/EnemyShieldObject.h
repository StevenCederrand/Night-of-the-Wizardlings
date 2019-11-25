#ifndef _ENEMY_SHIELD_OBJECT_H
#define _ENEMY_SHIELD_OBJECT_H
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>

class EnemyShieldObject : public GameObject {
public:
	EnemyShieldObject();
	EnemyShieldObject(std::string name);
	virtual ~EnemyShieldObject();

	void update(float dt);
private:

};

#endif // !_DEFLECT_OBJECT_H
