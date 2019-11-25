#ifndef _SHIELD_OBJECT_H
#define _SHIELD_OBJECT_H
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>

class ShieldObject : public GameObject {
public:
	ShieldObject();
	ShieldObject(std::string name);
	virtual ~ShieldObject();

	void update(float dt);
private:

};

#endif // !_DEFLECT_OBJECT_H
