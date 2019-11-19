#include "Pch/Pch.h"
#include "SoundHandler.h"
#include "vorbis/vorbisfile.h"

SoundHandler* SoundHandler::m_soundHandlerInstance = 0;

SoundHandler::SoundHandler()
{	
	m_nrOfPlayers = 1;	
	
	m_buffersClient.resize(NR_OF_CLIENT_SOUNDS);	
	m_buffersCommon.resize(NR_OF_COMMON_SOUNDS);
	m_clientSoundInfo.sources.resize(NR_OF_CLIENT_SOUNDS);
	
	m_device = alcOpenDevice(NULL);

	if (!m_device)
	{
		logTrace("Could not open sound device");
	}

	m_context = alcCreateContext(m_device, NULL);	

	if (!alcMakeContextCurrent(m_context))
	{
		logTrace("Failed to make context current");
	}	

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{		
		logTrace("Failed to make context current");
	}

	loadAllSound();
	setPlayerGUIDs();
	attachBuffersToClientSources();
	for (int i = 0; i < m_nrOfPlayers; i++)
	{
		attachBuffersToPlayerSources(m_playerSoundInfo.at(i).guid);
	}
	
	//Set the volume for the client sources. 
	RakNet::AddressOrGUID myGuid = Client::getInstance()->getMyData().guid;	
	setSourceGain(0.8, BasicAttackSound, myGuid);
	setSourceGain(0.2, DeflectSound, myGuid);
	setSourceGain(0.2, EnhanceAttackSound, myGuid);
	setSourceGain(0.3, JumpSound, myGuid);
	setSourceGain(0.3, StepsSound, myGuid);
	setSourceGain(0.3, FireSound, myGuid);
	setSourceGain(0.3, TakingDamageSound);
	setSourceGain(0.3, HitmarkSound);	
}

SoundHandler::~SoundHandler()
{
	for (int i = 0; i < m_nrOfPlayers; i++)
	{
		for (int j = 0; j < m_playerSoundInfo.at(i).sources.size(); j++)
		{
			for (int k = 0; k < NR_OF_SUBSEQUENT_SOUNDS; k++)
			{
				alSourcei(m_playerSoundInfo.at(i).sources.at(j).at(k), AL_BUFFER, NULL);
				alDeleteSources(1, &m_playerSoundInfo.at(i).sources.at(j).at(k));
			}
		}
	}	

	for (int i = 0; i < m_clientSoundInfo.sources.size(); i++)
	{
		alSourcei(m_clientSoundInfo.sources.at(i), AL_BUFFER, NULL);
		alDeleteSources(1, &m_clientSoundInfo.sources.at(i));
	}	

	for (int i = 0; i < m_buffersClient.size(); i++)
	{		
		alDeleteBuffers(1, &m_buffersClient[i]);
	}	

	for (int i = 0; i < m_buffersCommon.size(); i++)
	{
		alDeleteBuffers(1, &m_buffersCommon[i]);
	}

	//m_context = alcGetCurrentContext();
	m_device = alcGetContextsDevice(m_context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(m_context);
	alcCloseDevice(m_device);	
}

SoundHandler* SoundHandler::getInstance()
{
	if (m_soundHandlerInstance == 0)
	{
		m_soundHandlerInstance = new SoundHandler();
	}

	return m_soundHandlerInstance;
}

void SoundHandler::destroy()
{
	delete m_soundHandlerInstance;
}

void SoundHandler::loadAllSound()
{
	int success;

	success = loadSound(ThemeSong0);
	if (success == -1)
	{
		logTrace(THEME_SONG0 + " failed to be loaded");
	}

	success = loadSound(BasicAttackSound);
	if (success == -1)
	{
		logTrace(BASIC_ATTACK_SOUND + " failed to be loaded");
	}

	success = loadSound(DeflectSound);
	if (success == -1)
	{
		logTrace(DEFLECT_SOUND + " failed to be loaded");
	}

	success = loadSound(EnhanceAttackSound);
	if (success == -1)
	{
		logTrace(ENHANCE_ATTACK_SOUND + " failed to be loaded");
	}	

	success = loadSound(TakingDamageSound);
	if (success == -1)
	{
		logTrace(TAKING_DAMAGE_SOUND + " failed to be loaded");
	}

	success = loadSound(StepsSound);
	if (success == -1)
	{
		logTrace(STEPS_SOUND + " failed to be loaded");
	}

	success = loadSound(JumpSound);
	if (success == -1)
	{
		logTrace(JUMP_SOUND + " failed to be loaded");
	}

	success = loadSound(HitmarkSound);
	if (success == -1)
	{
		logTrace(HITMARK_SOUND + " failed to be loaded");
	}

	success = loadSound(FireSound);
	if (success == -1)
	{
		logTrace(FIRE_SOUND + " failed to be loaded");
	}

	success = loadSound(GlassBreakSound);
	if (success == -1)
	{
		logTrace(GLASS_BREAK_SOUND + " failed to be loaded");
	}
}

int SoundHandler::loadSound(SoundIndexClient whatSound)
{
	std::string fileName = SOUNDEFFECTPATH;

	switch (whatSound)
	{
	case ThemeSong0:
		fileName += THEME_SONG0;
		break;		
	case TakingDamageSound:
		fileName += TAKING_DAMAGE_SOUND;
		break;		
	case PickupSpawnSound:
		break;
	case HitmarkSound:
		fileName += HITMARK_SOUND;
		break;	
	}

	FILE* fp = 0;
	OggVorbis_File vf;
	vorbis_info* vInfo;
	ALenum format = 0;
	short* pcmout = 0;

	//Open sound file for reading
	fopen_s(&fp, fileName.c_str(), "rb");

	if (fp == 0)
	{
		logTrace("Could not open sound file");

		return -1;
	}

	//Generate buffer
	m_error = alGetError();

	alGenBuffers((ALsizei)1, &m_buffersClient.at(whatSound));

	m_error = alGetError();

	if (m_error != AL_NO_ERROR)
	{
		logTrace("Error generating buffers");

		free(pcmout);
		fclose(fp);
		return -1;
	}

	//Open ogg file
	if (ov_open_callbacks(fp, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)
	{
		logTrace("Stream is not a valid OggVorbis stream!");

		free(pcmout);
		fclose(fp);
		ov_clear(&vf);
		return -1;
	}

	//Fill vInfo with a new ogg vorbis info struct, determine audio format
	vInfo = ov_info(&vf, -1);
	format = vInfo->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;	

	size_t data_len = ov_pcm_total(&vf, -1) * vInfo->channels * 2; // 16bits
	pcmout = (short*)malloc(data_len);
	if (pcmout == 0)
	{
		logTrace("Out of memory");

		free(pcmout);
		fclose(fp);
		ov_clear(&vf);
		return -1;
	}

	//Fill pcmout buffer with ov_read data samples
	for (size_t size = 0, offset = 0, sel = 0;
		(size = ov_read(&vf, (char*)pcmout + offset, 4096, 0, 2, 1, (int*)& sel)) != 0;
		offset += size)
	{
		if (size < 0)
		{
			logTrace("Something wrong with the ogg file");

			free(pcmout);
			fclose(fp);
			ov_clear(&vf);
			return -1;
		}
	}

	m_error = alGetError();
	//send data to openal, vInfo->rate is your freq in Hz
	alBufferData(m_buffersClient[whatSound], format, pcmout, data_len, vInfo->rate);
	if ((m_error = alGetError() != AL_NO_ERROR))
	{
		logTrace("Failed to send audio information buffer to OpenAL");

		free(pcmout);
		fclose(fp);
		ov_clear(&vf);
		return -1;
	}	

	free(pcmout);
	fclose(fp);
	ov_clear(&vf);

	return whatSound;
}

int SoundHandler::loadSound(SoundIndexCommon whatSound)
{
	std::string fileName = SOUNDEFFECTPATH;

	switch (whatSound)
	{	
	case BasicAttackSound:
		fileName += BASIC_ATTACK_SOUND;
		break;
	case DeflectSound:
		fileName += DEFLECT_SOUND;
		break;
	case EnhanceAttackSound:
		fileName += ENHANCE_ATTACK_SOUND;
		break;	
	case StepsSound:
		fileName += STEPS_SOUND;
		break;
	case JumpSound:
		fileName += JUMP_SOUND;
		break;	
	case FireSound:
		fileName += FIRE_SOUND;
		break;
	case GlassBreakSound:
		fileName += GLASS_BREAK_SOUND;
	}

	FILE* fp = 0;
	OggVorbis_File vf;
	vorbis_info* vInfo;
	ALenum format = 0;
	short* pcmout = 0;

	//Open sound file for reading
	fopen_s(&fp, fileName.c_str(), "rb");

	if (fp == 0)
	{
		logTrace("Could not open sound file");

		return -1;
	}

	//Generate buffer
	m_error = alGetError();

	alGenBuffers((ALsizei)1, &m_buffersCommon[whatSound]);

	m_error = alGetError();

	if (m_error != AL_NO_ERROR)
	{
		logTrace("Error generating buffers");

		free(pcmout);
		fclose(fp);
		return -1;
	}

	//Open ogg file
	if (ov_open_callbacks(fp, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)
	{
		logTrace("Stream is not a valid OggVorbis stream!");

		free(pcmout);
		fclose(fp);
		ov_clear(&vf);
		return -1;
	}

	//Fill vInfo with a new ogg vorbis info struct, determine audio format
	vInfo = ov_info(&vf, -1);
	format = vInfo->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
	//format = AL_FORMAT_MONO16;

	size_t data_len = ov_pcm_total(&vf, -1) * vInfo->channels * 2; // 16bits
	pcmout = (short*)malloc(data_len);
	if (pcmout == 0)
	{
		logTrace("Out of memory");

		free(pcmout);
		fclose(fp);
		ov_clear(&vf);
		return -1;
	}

	//Fill pcmout buffer with ov_read data samples
	for (size_t size = 0, offset = 0, sel = 0;
		(size = ov_read(&vf, (char*)pcmout + offset, 4096, 0, 2, 1, (int*)& sel)) != 0;
		offset += size)
	{
		if (size < 0)
		{
			logTrace("Something wrong with the ogg file");

			free(pcmout);
			fclose(fp);
			ov_clear(&vf);
			return -1;
		}
	}

	m_error = alGetError();
	//send data to openal, vInfo->rate is your freq in Hz
	alBufferData(m_buffersCommon[whatSound], format, pcmout, data_len, vInfo->rate);
	if ((m_error = alGetError() != AL_NO_ERROR))
	{
		logTrace("Failed to send audio information buffer to OpenAL");

		free(pcmout);
		fclose(fp);
		ov_clear(&vf);
		return -1;
	}

	free(pcmout);
	fclose(fp);
	ov_clear(&vf);

	return whatSound;
}
void SoundHandler::setPlayerSourceGains(RakNet::AddressOrGUID guid)
{
	bool found = false;

	for (int i = 0; i < m_playerSoundInfo.size() && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == guid.rakNetGuid)
		{
			setSourceGain(0.8, BasicAttackSound, m_playerSoundInfo.at(i).guid);
			setSourceGain(0.2, DeflectSound, m_playerSoundInfo.at(i).guid);
			setSourceGain(0.2, EnhanceAttackSound, m_playerSoundInfo.at(i).guid);
			setSourceGain(0.3, JumpSound, m_playerSoundInfo.at(i).guid);
			setSourceGain(0.4, StepsSound, m_playerSoundInfo.at(i).guid);
			setSourceGain(0.3, FireSound, m_playerSoundInfo.at(i).guid);
			found = true;
		}
	}	
}
void SoundHandler::attachBuffersToClientSources()
{	
	for (int i = 0; i < NR_OF_CLIENT_SOUNDS; i++)
	{
		m_error = alGetError();
		alGenSources((ALsizei)1, &m_clientSoundInfo.sources.at(i));
		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error generating sources");			
		}

		m_error = alGetError();
		alSourcei(m_clientSoundInfo.sources.at(i), AL_BUFFER, m_buffersClient.at(i));
		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error binding buffer to source");
		}
	}	
}

//Create sources for every player's spells and steps.
//Returns 0 when successful, returns -1 when the player ID could not be found.
int SoundHandler::attachBuffersToPlayerSources(RakNet::AddressOrGUID playerID)
{		
	int success = 0;
	m_error = alGetError();
	PlayerSoundInfo* playerTemp = nullptr;
	bool found = false;
	int index = -1;
	for (int i = 0; i < m_playerSoundInfo.size(); i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
		{
			playerTemp = &m_playerSoundInfo.at(i);
			found = true;
			index = i;
		}
	}
	if (found)
	{
		playerTemp->sources.resize(NR_OF_COMMON_SOUNDS);
		for (int i = 0; i < NR_OF_COMMON_SOUNDS; i++)
		{
			playerTemp->sources.at(i).resize(NR_OF_SUBSEQUENT_SOUNDS);
			for (int j = 0; j < NR_OF_SUBSEQUENT_SOUNDS; j++)
			{
				alGenSources((ALsizei)1, &playerTemp->sources.at(i).at(j));

				if ((m_error = alGetError()) != AL_NO_ERROR)
				{
					logTrace("Error generating sources");
				}

				m_error = alGetError();
				alSourcei(playerTemp->sources.at(i).at(j), AL_BUFFER, m_buffersCommon.at(i));

				if ((m_error = alGetError()) != AL_NO_ERROR)
				{
					logTrace("Error binding buffer to source");
				}
			}
		}				
	}	
	else
	{
		success = -1;
	}

	return success;
}

void SoundHandler::playSound(SoundIndexClient whatSound)
{			
	m_error = alGetError();
	alSourcePlay(m_clientSoundInfo.sources.at(whatSound));

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error playing sound");
	}			
}

int SoundHandler::playSound(SoundIndexCommon whatSound, RakNet::AddressOrGUID playerID)
{
	int slot = -1;
	bool found = false;
	for (int i = 0; i < m_nrOfPlayers && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
		{
			//Sounds that should be able to play over each other
			if (whatSound == JumpSound || whatSound == BasicAttackSound ||
				whatSound == EnhanceAttackSound || whatSound == FireSound ||
				whatSound == GlassBreakSound)
			{
				m_error = alGetError();
				bool foundFreeSlot = false;

				for (int j = 0; j < NR_OF_SUBSEQUENT_SOUNDS && !foundFreeSlot; j++)
				{
					if (getSourceState(whatSound, playerID, j) != AL_PLAYING)
					{
						alSourcePlay(m_playerSoundInfo.at(i).sources.at(whatSound).at(j));
						foundFreeSlot = true;
						slot = j;						
					}
				}				

				if ((m_error = alGetError()) != AL_NO_ERROR)
				{
					logTrace("Error playing sound");
				}				
			}
			//Sounds that are not supposed to play more than one at a time
			else if (getSourceState(whatSound, playerID) != AL_PLAYING)
			{
				m_error = alGetError();
				alSourcePlay(m_playerSoundInfo.at(i).sources.at(whatSound).at(0));

				if ((m_error = alGetError()) != AL_NO_ERROR)
				{
					logTrace("Error playing sound");
				}
				slot = 0;
			}
			found = true;
		}
	}

	return slot;
}

void SoundHandler::pauseSound(SoundIndexClient whatSound)
{	
	m_error = alGetError();

	if (getSourceState(whatSound) == AL_PLAYING)
	{
		alSourcePause(m_clientSoundInfo.sources.at(whatSound));
	}

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error pausing sound");
	}	
}

void SoundHandler::pauseSound(SoundIndexCommon whatSound, RakNet::AddressOrGUID playerID)
{
	bool found = false;
	for (int i = 0; i < m_nrOfPlayers && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
		{
			m_error = alGetError();

			if (getSourceState(whatSound, playerID) == AL_PLAYING)
			{
				//Assume it's source one. If necessary, loop through every source slot instead.
				alSourcePause(m_playerSoundInfo.at(i).sources.at(whatSound).at(0)); 
			}

			if ((m_error = alGetError()) != AL_NO_ERROR)
			{
				logTrace("Error pausing sound");
			}

			found = true;
		}
	}
}

void SoundHandler::stopSound(SoundIndexClient whatSound)
{	
	m_error = alGetError();

	if (getSourceState(whatSound) == AL_PLAYING)
	{
		alSourceStop(m_clientSoundInfo.sources.at(whatSound));
	}

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error stopping sound");
	}	
}

void SoundHandler::stopSound(SoundIndexCommon whatSound, RakNet::AddressOrGUID playerID, int slot)
{
	bool found = false;
	for (int i = 0; i < m_nrOfPlayers && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
		{
			m_error = alGetError();

			if (getSourceState(whatSound, playerID) == AL_PLAYING)
			{				
				alSourceStop(m_playerSoundInfo.at(i).sources.at(whatSound).at(slot)); 
			}

			if ((m_error = alGetError()) != AL_NO_ERROR)
			{
				logTrace("Error stopping sound");
			}

			found = true;
		}
	}
}

void SoundHandler::setListenerPos(glm::vec3 pos)
{
	ALfloat listenerPos[] = { pos.x, pos.y, pos.z };

	m_error = alGetError();

	alListenerfv(AL_POSITION, listenerPos);
	
	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Couldn't set listener position");
	}
}

void SoundHandler::setListenerVelocity(glm::vec3 vel)
{	
	ALfloat listenerVel[] = { vel.x, vel.y, vel.z };
	
	m_error = alGetError();

	alListenerfv(AL_VELOCITY, listenerVel);
	
	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Couldn't set listener velocity");
	}
}

void SoundHandler::setListenerOrientation(glm::vec3 lookAt, glm::vec3 up)
{	
	ALfloat listenerOri[] = { lookAt.x, lookAt.y, lookAt.z, up.x, up.y, up.z };
	
	m_error = alGetError();

	alListenerfv(AL_ORIENTATION, listenerOri);
	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Couldn't set listener orientation");
	}
}

void SoundHandler::setSourcePitch(float pitch, SoundIndexClient whatSound)
{	
	m_error = alGetError();

	alSourcef(m_clientSoundInfo.sources.at(whatSound), AL_PITCH, pitch);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting pitch to source");
	}
}

void SoundHandler::setSourcePitch(float pitch, SoundIndexCommon whatSound, RakNet::AddressOrGUID playerID, int slot)
{
	bool found = false;
	for (int i = 0; i < m_nrOfPlayers && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
		{			
			m_error = alGetError();

			alSourcef(m_playerSoundInfo.at(i).sources.at(whatSound).at(slot), AL_PITCH, pitch);

			if ((m_error = alGetError()) != AL_NO_ERROR)
			{
				logTrace("Error setting pitch to source");
			}
			
			found = true;
		}
	}
}

void SoundHandler::setSourceGain(float gain, SoundIndexClient whatSound)
{	
	m_error = alGetError();

	alSourcef(m_clientSoundInfo.sources.at(whatSound), AL_GAIN, gain);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting gain to source");
	}	
}

void SoundHandler::setSourceGain(float gain, SoundIndexCommon whatSound, RakNet::AddressOrGUID playerID, int slot)
{
	bool found = false;
	for (int i = 0; i < m_nrOfPlayers && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
		{
			
			m_error = alGetError();

			alSourcef(m_playerSoundInfo.at(i).sources.at(whatSound).at(slot), AL_GAIN, gain);

			if ((m_error = alGetError()) != AL_NO_ERROR)
			{
				logTrace("Error setting gain to source");
			}
			
			found = true;
		}
	}
}

void SoundHandler::setSourceMaxGain(float gain, SoundIndexClient whatSound)
{
	m_error = alGetError();

	alSourcef(m_clientSoundInfo.sources.at(whatSound), AL_MAX_GAIN, gain);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting gain to source");
	}
}

void SoundHandler::setSourceMaxGain(float gain, SoundIndexCommon whatSound, RakNet::AddressOrGUID playerID, int slot)
{
	bool found = false;
	for (int i = 0; i < m_nrOfPlayers && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
		{
			
			m_error = alGetError();

			alSourcef(m_playerSoundInfo.at(i).sources.at(whatSound).at(slot), AL_MAX_GAIN, gain);

			if ((m_error = alGetError()) != AL_NO_ERROR)
			{
				logTrace("Error setting gain to source");
			}
			
			found = true;
		}
	}
}

void SoundHandler::setSourceMaxDistance(float dist, SoundIndexClient whatSound)
{	
	m_error = alGetError();

	alSourcef(m_clientSoundInfo.sources.at(whatSound), AL_MAX_DISTANCE, dist);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting max distance to source");
	}	
}

void SoundHandler::setSourceMaxDistance(float dist, SoundIndexCommon whatSound, RakNet::AddressOrGUID playerID, int slot)
{
	bool found = false;
	for (int i = 0; i < m_nrOfPlayers && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
		{
			
			m_error = alGetError();

			alSourcef(m_playerSoundInfo.at(i).sources.at(whatSound).at(slot), AL_MAX_DISTANCE, dist);

			if ((m_error = alGetError()) != AL_NO_ERROR)
			{
				logTrace("Error setting max distance to source");
			}			

			found = true;
		}
	}
}

void SoundHandler::setSourcePosition(glm::vec3 pos, SoundIndexClient whatSound)
{	
	ALfloat sourcePosition[] = { pos.x, pos.y, pos.z };	
	m_error = alGetError();

	alSourcefv(m_clientSoundInfo.sources.at(whatSound), AL_POSITION, sourcePosition);
	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting position to source");
	}	
}

void SoundHandler::setSourcePosition(glm::vec3 pos, SoundIndexCommon whatSound, RakNet::AddressOrGUID playerID, int slot)
{
	ALfloat sourcePosition[] = { pos.x, pos.y, pos.z };
	bool found = false;
	for (int i = 0; i < m_nrOfPlayers && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
		{
			m_error = alGetError();
			
			alSourcefv(m_playerSoundInfo.at(i).sources.at(whatSound).at(slot), AL_POSITION, sourcePosition);
						
			if ((m_error = alGetError()) != AL_NO_ERROR)
			{
				logTrace("Error setting position to source");
			}
			found = true;
		}				
	}
}

void SoundHandler::setSourceVelocity(glm::vec3 vel, SoundIndexClient whatSound)
{	
	ALfloat sourceVelocity[] = { vel.x, vel.y, vel.z };	

	m_error = alGetError();

	alSourcefv(m_clientSoundInfo.sources.at(whatSound), AL_VELOCITY, sourceVelocity);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting velocity to source");
	}	
}

void SoundHandler::setSourceVelocity(glm::vec3 vel, SoundIndexCommon whatSound, RakNet::AddressOrGUID playerID, int slot)
{
	ALfloat sourceVelocity[] = { vel.x, vel.y, vel.z };
	bool found = false;

	for (int i = 0; i < m_nrOfPlayers && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
		{			
			m_error = alGetError();

			alSourcefv(m_playerSoundInfo.at(i).sources.at(whatSound).at(slot), AL_VELOCITY, sourceVelocity);

			if ((m_error = alGetError()) != AL_NO_ERROR)
			{
				logTrace("Error setting velocity to source");
			}

			found = true;
		}
	}
}

void SoundHandler::setSourceDirection(glm::vec3 dir, SoundIndexClient whatSound)
{	
	ALfloat sourceDirection[] = { dir.x, dir.y, dir.z };	

	m_error = alGetError();

	alSourcefv(m_clientSoundInfo.sources.at(whatSound), AL_DIRECTION, sourceDirection);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting direction to source");
	}	
}

void SoundHandler::setSourceDirection(glm::vec3 dir, SoundIndexCommon whatSound, RakNet::AddressOrGUID playerID, int slot)
{
	ALfloat sourceDirection[] = { dir.x, dir.y, dir.z };
	bool found = false;

	for (int i = 0; i < m_nrOfPlayers && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
		{
			
			m_error = alGetError();

			alSourcefv(m_playerSoundInfo.at(i).sources.at(whatSound).at(slot), AL_DIRECTION, sourceDirection);

			if ((m_error = alGetError()) != AL_NO_ERROR)
			{
				logTrace("Error setting direction to source");
			}
			
			found = true;
		}
	}
}

//Types are: AL_UNDETERMINED, AL_STATIC or AL_STREAMING
void SoundHandler::setSourceType(ALenum type, SoundIndexClient whatSound)
{	
	m_error = alGetError();
	alSourcei(m_clientSoundInfo.sources.at(whatSound), AL_SOURCE_TYPE, type);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting type to source");
	}	
}

//Types are: AL_UNDETERMINED, AL_STATIC or AL_STREAMING
void SoundHandler::setSourceType(ALenum type, SoundIndexCommon whatSound, RakNet::AddressOrGUID playerID, int slot)
{
	bool found = false;
	for (int i = 0; i < m_nrOfPlayers && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
		{			
			m_error = alGetError();
			alSourcei(m_playerSoundInfo.at(i).sources.at(whatSound).at(slot), AL_SOURCE_TYPE, type);

			if ((m_error = alGetError()) != AL_NO_ERROR)
			{
				logTrace("Error setting type to source");
			}
			
			found = true;
		}
	}
}

void SoundHandler::setSourceLooping(bool looping, SoundIndexClient whatSound)
{	
	
	m_error = alGetError();

	alSourcei(m_clientSoundInfo.sources.at(whatSound), AL_LOOPING, looping);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting looping value to source");
	}	
}

void SoundHandler::setSourceLooping(bool looping, SoundIndexCommon whatSound, RakNet::AddressOrGUID playerID, int slot)
{
	bool found = false;
	for (int i = 0; i < m_nrOfPlayers && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
		{			
			m_error = alGetError();

			alSourcei(m_playerSoundInfo.at(i).sources.at(whatSound).at(slot), AL_LOOPING, looping);

			if ((m_error = alGetError()) != AL_NO_ERROR)
			{
				logTrace("Error setting looping value to source");
			}
			
			found = true;
		}
	}
}

void SoundHandler::setPlayerGUIDs()
{	
	auto clientPtr = Client::getInstance();
	auto& list = clientPtr->getConnectedPlayers();
	PlayerPacket myDataPacket = clientPtr->getMyData();
	
	if (m_playerSoundInfo.size() < 1)
	{
		m_playerSoundInfo.resize(1);
		m_playerSoundInfo.at(0).sources.resize(NR_OF_SUBSEQUENT_SOUNDS);
	}

	m_playerSoundInfo.at(0).guid = myDataPacket.guid;

	for (int i = 1; i < list.size() + 1; i++)
	{
		if (m_playerSoundInfo.size() < i + 1)
		{
			m_playerSoundInfo.resize(m_playerSoundInfo.size() + 1);
			//m_playerSoundInfo.at(m_playerSoundInfo.size() - 1).sources.resize(NR_OF_SUBSEQUENT_SOUNDS);
		}
		
		m_playerSoundInfo[i].guid = list[i].guid;
	}	

	m_nrOfPlayers = list.size() + 1;

	
	if (list.size() != 0)
	{
		for (int i = 0; i < m_playerSoundInfo.size(); i++)
			setPlayerSourceGains(m_playerSoundInfo.at(i).guid);
	}
}

void SoundHandler::addPlayer(RakNet::AddressOrGUID guid)
{
	m_playerSoundInfo.resize(m_playerSoundInfo.size() + 1);
	//m_playerSoundInfo.at(m_playerSoundInfo.size() - 1).sources.resize(NR_OF_SUBSEQUENT_SOUNDS);
	m_playerSoundInfo.at(m_playerSoundInfo.size() - 1).guid = guid;
	m_nrOfPlayers++;
	attachBuffersToPlayerSources(guid);
	setPlayerSourceGains(guid);
}

void SoundHandler::removePlayer(RakNet::AddressOrGUID guid)
{
	int index = -1;
	bool found = false;
	for (int i = 0; i < m_playerSoundInfo.size() && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == guid.rakNetGuid)
		{
			index = i;
			for (int j = 0; j < m_playerSoundInfo.at(i).sources.size(); j++)
			{
				for (int k = 0; k < NR_OF_SUBSEQUENT_SOUNDS; k++)
				{
					alSourcei(m_playerSoundInfo.at(i).sources.at(j).at(k), AL_BUFFER, NULL);
					alDeleteSources(1, &m_playerSoundInfo.at(i).sources.at(j).at(k));
				}
			}
			found = true;
		}
	}

	if (found)
	{
		for (int i = index; i < m_playerSoundInfo.size() - 1; i++)
		{
			m_playerSoundInfo.at(i) = m_playerSoundInfo.at(i + 1);
		}

		m_playerSoundInfo.resize(m_playerSoundInfo.size() - 1);

		m_nrOfPlayers--;
	}
}

const ALint& SoundHandler::getSourceState(SoundIndexClient whatSound) const
{
	ALint value;	
	
	alGetSourcei(m_clientSoundInfo.sources.at(whatSound), AL_SOURCE_STATE, &value);
	
	return value;
}

const ALint& SoundHandler::getSourceState(SoundIndexCommon whatSound, RakNet::AddressOrGUID playerID, int slot) const
{
	ALint value;
	bool found = false;
	for (int i = 0; i < m_playerSoundInfo.size() && !found; i++)
	{
		if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
		{
			//Assume source one, change if necessary
			alGetSourcei(m_playerSoundInfo.at(i).sources.at(whatSound).at(slot), AL_SOURCE_STATE, &value);
			found = true;
		}
	}

	return value;
}
