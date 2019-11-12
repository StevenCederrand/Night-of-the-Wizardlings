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

//Nr of sounds in total
const int NR_OF_SOUNDS = 7;
//Nr of sounds every player has in common.
const int NR_OF_COMMON_SOUNDS = 5;

//Always put sounds that will come only from this client at the bottom of the list.
//Put sounds that you will hear from other players aswell at the top of the list.
//Otherwise the will be problems when creating sources for the sounds 
//everyone has in common.
//Also, make sure to increase NR_OF_SOUNDS
//Make sure to increase NR_OF_COMMON_SOUNDS if it is a sound that you can hear 
//from other players aswell. For example, you can hear other players' spells.
enum SoundIndex {	
	//Sounds in common here:
	BasicAttackSound,
	DeflectSound,
	EnhanceAttackSound,
	TakingDamageSound,
	StepsSound,
	//Sounds only for client here:
	ThemeSong0,
	PickupSpawnSound,	
};

struct PlayerSoundInfo
{
	RakNet::AddressOrGUID guid;	
	std::vector<ALuint> sources;
};

class SoundHandler
{
private:
	static SoundHandler* m_soundHandlerInstance;
	ALCdevice* m_device;
	ALCcontext* m_context;
	std::vector<ALuint> m_buffers;		
	std::vector<PlayerSoundInfo> m_playerSoundInfo;	
	ALuint themeSong0Source;
	ALuint pickupSpawnSource;
	ALenum m_error;
	int m_nrOfPlayers; // Including me

public:
	SoundHandler();
	~SoundHandler();

	static SoundHandler* getInstance();

	void destroy();

	void loadAllSound();
	int loadSound(SoundIndex whatSound);
	int attachBuffersToPlayerSources(RakNet::AddressOrGUID playerID);

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
	void addPlayer(RakNet::AddressOrGUID guid);
	void removePlayer(RakNet::AddressOrGUID guid);

	const ALint& getSourceState(SoundIndex bufferName, RakNet::AddressOrGUID playerID) const;
	//source relative?	
};

#endif
