#ifndef TANGENTCUBE_H
#define TANGENTCUBE_H
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
class TangentCube : public GameObject
{
public:
	TangentCube();
	~TangentCube();

	//void loadNormalMap();	

	void update(float dt);
private:
	
};

#endif