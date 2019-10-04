#pragma once
#ifndef _ANIMATEDOBJECT_h
#define _ANIMATEDOBJECT_h
#include <GameObject/GameObject.h>

class AnimatedObject : public GameObject
{
public:
	AnimatedObject(std::string name);
	virtual ~AnimatedObject();

	void update(float dt);
	void ComputeMatrix(int meshId, std::string meshn, std::string animation, float dt);
	void BindMatrix(int meshId);

private:
	struct BonePalleteBuffer
	{
		glm::mat4 bones[64]{ glm::mat4() };
	};

	std::vector<BonePalleteBuffer> bonePallete;
	GLuint boneBuffer;

	float currentTime;

};

#endif

