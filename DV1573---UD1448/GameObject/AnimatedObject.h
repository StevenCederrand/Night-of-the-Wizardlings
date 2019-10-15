#ifndef _ANIMATEDOBJECT_h
#define _ANIMATEDOBJECT_h
#include <GameObject/GameObject.h>
class AnimatedObject : public GameObject
{
public:
	AnimatedObject(std::string name);
	virtual ~AnimatedObject();

	void update(float dt);

	//Calculates the transforms of a joint at a specific time, also interpolates.
	void ComputeMatrix(int meshId, std::string meshn, std::string animation);

	//Binds the transforms to the shader
	void BindAnimation(int meshId);

private:

	//The bone pallete holds the calculated transforms for a specific joint (bone)
	BonePalleteBuffer bonePallete;
	GLuint boneBuffer;

	float currentTime;

};

#endif

