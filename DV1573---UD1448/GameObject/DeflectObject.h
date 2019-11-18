#ifndef _DEFLECT_OBJECT_H
#define _DEFLECT_OBJECT_H
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>

class DeflectObject : public GameObject {
public:
	DeflectObject();
	DeflectObject(std::string name);
	virtual ~DeflectObject();

	void update(float dt);
private:

};

#endif // !_DEFLECT_OBJECT_H
