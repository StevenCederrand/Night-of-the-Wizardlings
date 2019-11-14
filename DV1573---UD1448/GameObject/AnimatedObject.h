#ifndef _ANIMATEDOBJECT_h
#define _ANIMATEDOBJECT_h
#include <GameObject/GameObject.h>
class AnimatedObject : public GameObject
{
private:
	struct frameAnimation
	{
		std::string m_name;
		float m_startTime;
		float m_stopTime;
		float m_animSpeed;
		frameAnimation()
		{
			m_startTime = 0;
			m_stopTime = 0;
			m_animSpeed = 24;
		}
	};

public:
	AnimatedObject(std::string name);
	virtual ~AnimatedObject();

	void update(float dt);

	//Calculates the transforms of a joint at a specific time, also interpolates.
	void ComputeMatrix(int meshId, std::string meshn, std::string animation);

	//Binds the transforms to the shader
	void BindAnimation(int meshId);

	void initAnimations(std::string name, float startTime, float stopTime);

	void playAnimation(std::string name);

	void playLoopAnimation(std::string name);


private:


	std::vector<frameAnimation> animations;

	//The bone pallete holds the calculated transforms for a specific joint (bone)
	BonePalleteBuffer bonePallete;
	GLuint boneBuffer;
	float m_stopTime;
	float m_startTime;
	float currentTime;
	float tempTime;
	std::string currentAnimation;

	bool isLooping = false;
	bool isDone = false;
	
};

#endif

