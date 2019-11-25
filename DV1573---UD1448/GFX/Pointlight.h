#ifndef _POINTLIGHT_h
#define _POINTLIGHT_h
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>

class Pointlight : public GameObject {
public: 
	Pointlight();
	Pointlight(glm::vec3 position, glm::vec3 color);
	virtual ~Pointlight();

	void update(float dt);
private: 
	glm::vec3 m_color;
};


#endif