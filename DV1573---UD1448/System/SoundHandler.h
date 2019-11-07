#ifndef SOUNDHANDLER_H
#define SOUNDHANDLER_H
#include <Pch/Pch.h>
#include <OpenAl/al.h>
#include <OpenAL/alc.h>

//Set custom names for easier reference to soundFile names
const std::string THEME_SONG0 = "HarryPotterThemeSong.ogg";
const std::string BASIC_ATTACK_SOUND = "Sound Effect Magic Sound HQ YouTube.ogg";
const std::string DEFLECT_SOUND = "YouShallNotPassogg.ogg";
const std::string ENHANCE_ATTACK_SOUND = "Magic sound effect.ogg";

const int NR_OF_SOUNDS = 7;

enum SoundIndex {
	ThemeSong0,
	BasicAttackSoundIndex,
	DeflectSoundIndex,
	EnhanceAttackSoundIndex,
	TakingDamageSoundIndex,
	PickupSpawnSoundIndex,
	StepsSoundIndex
};

class SoundHandler
{
private:
	static SoundHandler* m_soundHandlerInstance;
	ALCdevice* m_device;
	ALCcontext* m_context;
	std::vector<ALuint> m_buffers;	
	std::vector<ALuint> m_sources;
	ALenum m_error;

public:
	SoundHandler();
	~SoundHandler();

	static SoundHandler* getInstance();

	void destroy();

	void loadAllSound();
	int loadSound(SoundIndex whatSound);
	void playSound(SoundIndex sourceName);
	void pauseSound(SoundIndex sourceName);
	void stopSound(SoundIndex sourceName);

	void setListenerPos(glm::vec3 pos);
	void setListenerVelocity(glm::vec3 vel);
	void setListenerOrientation(glm::vec3 lookAt, glm::vec3 up);

	void setSourcePitch(float pitch, SoundIndex sourceName);
	void setSourceGain(float gain, SoundIndex sourceName);
	void setSourceMaxDistance(float dist, SoundIndex sourceName);
	void setSourcePosition(glm::vec3 pos, SoundIndex sourceName);
	void setSourceVelocity(glm::vec3 vel, SoundIndex sourceName);
	void setSourceDirection(glm::vec3 dir, SoundIndex sourceName);
	void setSourceType(ALenum type, SoundIndex sourceName); //AL_UNDETERMINED, AL_STATIC or AL_STREAMING
	void setSourceLooping(bool looping, SoundIndex sourceName);

	const ALint& getSourceState(SoundIndex sourceName) const;
	//source relative?	
};

#endif
