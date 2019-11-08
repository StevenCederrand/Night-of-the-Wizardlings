#ifndef SOUNDHANDLER_H
#define SOUNDHANDLER_H
#include <Pch/Pch.h>
#include <OpenAl/al.h>
#include <OpenAL/alc.h>
#include <Networking/Client.h>

//Set custom names for easier reference to soundFile names
const std::string THEME_SONG0 = "HarryPotterThemeSong.ogg";
const std::string BASIC_ATTACK_SOUND = "Sound Effect Magic Sound HQ YouTube.ogg";
const std::string DEFLECT_SOUND = "YouShallNotPassogg.ogg";
const std::string ENHANCE_ATTACK_SOUND = "Magic sound effect.ogg";

const int NR_OF_SOUNDS = 7;
const int MAX_NR_OF_PLAYERS = 5;

enum SoundIndex {
	ThemeSong0,
	BasicAttackSoundIndex,
	DeflectSoundIndex,
	EnhanceAttackSoundIndex,
	TakingDamageSoundIndex,
	PickupSpawnSoundIndex,
	StepsSoundIndex
};

struct PlayerSoundSourceInfo
{
	RakNet::AddressOrGUID guid;
	int basicAttackSourceIndex;
	int deflectSourceIndex;
	int enhanceSourceIndex;
	int takingDamageSourceIndex;
	int stepsSourceIndex;
};

class SoundHandler
{
private:
	static SoundHandler* m_soundHandlerInstance;
	ALCdevice* m_device;
	ALCcontext* m_context;
	std::vector<ALuint> m_buffers;	
	std::vector<ALuint> m_sources;
	std::vector<PlayerSoundSourceInfo> m_playerSourceIndices;
	int m_themeSong0SourceIndex;
	int m_pickupSpawnSourceIndex;
	ALenum m_error;

public:
	SoundHandler();
	~SoundHandler();

	static SoundHandler* getInstance();

	void destroy();

	void loadAllSound();
	int loadSound(SoundIndex whatSound);
	void playSound(SoundIndex bufferName, RakNet::AddressOrGUID playerID);
	void pauseSound(SoundIndex bufferName, RakNet::AddressOrGUID playerID);
	void stopSound(SoundIndex buffereName, RakNet::AddressOrGUID playerID);

	void setListenerPos(glm::vec3 pos);
	void setListenerVelocity(glm::vec3 vel);
	void setListenerOrientation(glm::vec3 lookAt, glm::vec3 up);

	void setSourcePitch(float pitch, SoundIndex bufferName, RakNet::AddressOrGUID playerID);
	void setSourceGain(float gain, SoundIndex bufferName, RakNet::AddressOrGUID playerID);
	void setSourceMaxDistance(float dist, SoundIndex bufferName, RakNet::AddressOrGUID playerID);
	void setSourcePosition(glm::vec3 pos, SoundIndex bufferName, RakNet::AddressOrGUID playerID);
	void setSourceVelocity(glm::vec3 vel, SoundIndex bufferName, RakNet::AddressOrGUID playerID);
	void setSourceDirection(glm::vec3 dir, SoundIndex bufferName, RakNet::AddressOrGUID playerID);
	void setSourceType(ALenum type, SoundIndex bufferName, RakNet::AddressOrGUID playerID); //AL_UNDETERMINED, AL_STATIC or AL_STREAMING
	void setSourceLooping(bool looping, SoundIndex bufferName, RakNet::AddressOrGUID playerID);

	void setPlayerGUIDs();

	const ALint& getSourceState(SoundIndex bufferName, RakNet::AddressOrGUID playerID) const;
	//source relative?	
};

#endif
