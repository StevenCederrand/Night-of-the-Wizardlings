#pragma once
#ifndef _ANIMATEDOBJECT_h
#define _ANIMATEDOBJECT_h
#include <GameObject/GameObject.h>

class AnimatedObject : public GameObject
{
public:
	AnimatedObject();
	AnimatedObject(std::string name);
	virtual ~AnimatedObject();

	void update(float dt);
	void ComputeMatrix(int animId, float dt);

private:
	struct SkinDataBuffer
	{
		glm::mat4 bones[64];
	};

	SkinDataBuffer skinData;

};

#endif

