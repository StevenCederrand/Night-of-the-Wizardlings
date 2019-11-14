#ifndef _DEFLECT_H
#define _DEFLECT_H
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>

class Deflect : public GameObject {
public:
	Deflect();
	Deflect(std::string name);
	virtual ~Deflect();

	void update(float dt);
private:

};

#endif // !_DEFLECT_H
