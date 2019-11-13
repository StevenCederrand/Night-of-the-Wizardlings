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
const std::string TAKING_DAMAGE_SOUND = "TakingDamage.ogg";
const std::string STEPS_SOUND = "Footsteps2.ogg";
const std::string JUMP_SOUND = "Jump1.ogg";
const std::string HITMARK_SOUND = "Hitmark.ogg";

//Nr of sounds every player has in common.
const int NR_OF_COMMON_SOUNDS = 6;
//Nr of sounds only the client will hear.
const int NR_OF_CLIENT_SOUNDS = 3;

//Put sounds that you will hear from yourself and other players as a SoundIndexCommon.
//For example, you can hear other players' spells.
//Put sounds that will come only from this client as a SoundIndexClient enum.
//Also, make sure to increase NR_OF_CLIENT_SOUNDS or NR_OF_COMMON_SOUNDS depending 
//on what type of sound you added.

//Sounds in common here:
enum SoundIndexCommon {		
	BasicAttackSound,
	DeflectSound,
	EnhanceAttackSound,
	TakingDamageSound,
	StepsSound,
	JumpSound	
};

//Sounds only for client here:
enum SoundIndexClient {	
	ThemeSong0,
	PickupSpawnSound,
	HitmarkSound
};

//Is it a sound only the client will hear (ClientSound) or
//is it a sound you can hear from other players aswell? (CommonSound)
struct ClientType
{
	SoundIndexClient bufferName;
};

struct CommonType
{
	SoundIndexCommon bufferName;
};

struct PlayerSoundInfo
{
	RakNet::AddressOrGUID guid;	
	std::vector<ALuint> sources;
};

struct ClientSoundInfo
{	
	std::vector<ALuint> sources;
};

class SoundHandler
{
private:
	ClientType* hej;
	static SoundHandler* m_soundHandlerInstance;
	ALCdevice* m_device;
	ALCcontext* m_context;
	std::vector<ALuint> m_buffers;		
	std::vector<PlayerSoundInfo> m_playerSoundInfo;	
	ALuint themeSong0Source;
	ALuint pickupSpawnSource;
	ALuint hitmarkSource;
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
