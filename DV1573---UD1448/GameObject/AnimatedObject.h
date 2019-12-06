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
	void ComputeMatrix(int meshId, std::string meshn, std::string animation, BonePalleteBuffer* bonePallete, float currentTime);
	void ComputeMatrixUpper(int meshId, std::string meshn, std::string animation, BonePalleteBuffer* bonePallete, float currentTime);
	void ComputeMatrixLower(int meshId, std::string meshn, std::string animation, BonePalleteBuffer* bonePallete, float currentTime);

	//Binds the transforms to the shader
	void BindAnimation(int meshId);

	void initAnimations(std::string name, float startTime, float stopTime);

	void playAnimation(std::string name);
	void playLoopAnimation(std::string name);
	void playUpperAnimation(std::string name);
	void playLowerAnimation(std::string name);

	void blendIdentity(float startTime, float stopTime, int index);

	void splitSkeleton(int upper);

private:

	int upperIndex;
	std::vector<frameAnimation> animations;

	//The bone pallete holds the calculated transforms for a specific joint (bone)
	BonePalleteBuffer m_bonePallete;
	BonePalleteBuffer m_bonePalleteBlend;

	GLuint m_boneBuffer;
	GLuint m_boneBufferBlend;

	float m_stopTime;
	float m_startTime;
	float m_currentTime;
	float m_tempTime;
	std::string m_currentAnimation;
	std::string m_currentAnimationLower;


	float m_stopTimeUpper;
	float m_startTimeUpper;
	float m_currentTimeUpper;

	float m_stopTimeLower;
	float m_startTimeLower;
	float m_currentTimeLower;

	bool m_isLooping = false;
	bool m_isDone = true;
	bool m_upper = false;//CREATE A BOOL FOR BOTH UPPER AND LOWER? (put compute matrix for upper and lower in same function?)
	bool m_lower = false;
	
};

#endif

