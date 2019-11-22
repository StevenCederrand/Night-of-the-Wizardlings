#ifndef _POINTLIGHT_h
#define _POINTLIGHT_h
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>

class Pointlight : public GameObject {
public: 
	Pointlight();
	Pointlight(glm::vec3 position, glm::vec3 color);


	virtual ~Pointlight();

	void setAttenuationAndRadius(const glm::vec4 attenAndRadius);
	void update(float dt);

	const glm::vec4& getAttenuationAndRadius() const;
private: 
	glm::vec3 m_color;
	
	glm::vec4 m_attenAndRadius; //First 3 dims are for the attenuation, final 4th is for radius
};


#endif