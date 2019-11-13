#include "Pch/Pch.h"
#include "SoundHandler.h"
#include "vorbis/vorbisfile.h"

SoundHandler* SoundHandler::m_soundHandlerInstance = 0;

SoundHandler::SoundHandler()
{	
	m_nrOfPlayers = 1;
	
	m_buffers.resize(NR_OF_SOUNDS);	

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

	m_error = alGetError();

	if (m_error != AL_NO_ERROR)
	{		
		logTrace("Failed to make context current");
	}

	loadAllSound();
	setPlayerGUIDs();
	for (int i = 0; i < m_nrOfPlayers; i++)
	{
		attachBuffersToPlayerSources(m_playerSoundInfo.at(i).guid);
	}

	//Lower the volume 
	RakNet::AddressOrGUID myGuid = Client::getInstance()->getMyData().guid;	
	setSourceGain(0.2, BasicAttackSound, myGuid);
	setSourceGain(0.2, DeflectSound, myGuid);
	setSourceGain(0.2, EnhanceAttackSound, myGuid);
	setSourceGain(0.3, JumpSound, myGuid);
	setSourceGain(0.3, StepsSound, myGuid);
	setSourceGain(0.3, TakingDamageSound, myGuid);
	setSourceGain(0.3, HitmarkSound, myGuid);

	hej = new ClientType();

	ClientType* lel = static_cast<ClientType*>(hej);


}

SoundHandler::~SoundHandler()
{
	alSourcei(themeSong0Source, AL_BUFFER, NULL);
	alSourcei(pickupSpawnSource, AL_BUFFER, NULL);
	alDeleteSources(1, &themeSong0Source);
	alDeleteSources(1, &pickupSpawnSource);

	for (int i = 0; i < m_nrOfPlayers; i++)
	{
		for (int j = 0; j < m_playerSoundInfo.at(i).sources.size(); j++)
		{
			alSourcei(m_playerSoundInfo.at(i).sources.at(j), AL_BUFFER, NULL);
			alDeleteSources(1, &m_playerSoundInfo.at(i).sources.at(j));
		}
	}	

	for (int i = 0; i < m_buffers.size(); i++)
	{		
		alDeleteBuffers(1, &m_buffers[i]);
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
}

//Returns the source name (position in the buffer array) which you use 
//when you call playSound(int sourceName). Or just use the enums.
//Returns -1 if failed
int SoundHandler::loadSound(SoundIndex whatSound)
{
	std::string fileName = SOUNDEFFECTPATH;		

	switch (whatSound)
	{
	case ThemeSong0:
		fileName += THEME_SONG0;		
		break;
	case BasicAttackSound:
		fileName += BASIC_ATTACK_SOUND;		
		break;
	case DeflectSound:
		fileName += DEFLECT_SOUND;
		break;
	case EnhanceAttackSound:
		fileName += ENHANCE_ATTACK_SOUND;
		break;
	case TakingDamageSound:
		fileName += TAKING_DAMAGE_SOUND;
		break;	
	case StepsSound:
		fileName += STEPS_SOUND;
		break;
	case JumpSound:
		fileName += JUMP_SOUND;
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

	alGenBuffers((ALsizei)1, &m_buffers[whatSound]);

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
	alBufferData(m_buffers[whatSound], format, pcmout, data_len, vInfo->rate);
	if ((m_error = alGetError() != AL_NO_ERROR))
	{
		logTrace("Failed to send audio information buffer to OpenAL");

		free(pcmout);
		fclose(fp);
		ov_clear(&vf);
		return -1;
	}
	
	//Theme song and pickup spawn sound only needs one source
	//because we don't need the enemies sounds for them. 
	m_error = alGetError();
	switch (whatSound)
	{
	case ThemeSong0:
		
		alGenSources((ALsizei)1, &themeSong0Source);	
		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error generating sources");

			free(pcmout);
			fclose(fp);
			ov_clear(&vf);
			return -1;
		}

		m_error = alGetError();
		alSourcei(themeSong0Source, AL_BUFFER, m_buffers[whatSound]);		
		break;
	case PickupSpawnSound:
		
		alGenSources((ALsizei)1, &pickupSpawnSource);
		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error generating sources");

			free(pcmout);
			fclose(fp);
			ov_clear(&vf);
			return -1;
		}

		m_error = alGetError();
		alSourcei(pickupSpawnSource, AL_BUFFER, m_buffers[whatSound]);	
		break;
	case HitmarkSound:
		
		alGenSources((ALsizei)1, &hitmarkSource);		

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error generating sources");

			free(pcmout);
			fclose(fp);
			ov_clear(&vf);
			return -1;
		}

		m_error = alGetError();
		alSourcei(hitmarkSource, AL_BUFFER, m_buffers[whatSound]);		
		break;
	}	
	
	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error binding buffer to source");

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
			alGenSources((ALsizei)1, &playerTemp->sources.at(i));
			m_error = alGetError();

			if (m_error != AL_NO_ERROR)
			{
				logTrace("Error generating sources");
			}
		}		

		for (int i = 0; i < NR_OF_COMMON_SOUNDS; i++)
		{
			m_error = alGetError();

			alSourcei(playerTemp->sources[i], AL_BUFFER, m_buffers[i]);

			m_error = alGetError();

			if (m_error != AL_NO_ERROR)
			{
				logTrace("Error binding buffer to source");
			}
		}			
	}	
	else
	{
		success = -1;
	}

	return success;
}

void SoundHandler::playSound(SoundIndex bufferName, RakNet::AddressOrGUID playerID)
{

	/*if (getSourceState(sourceName) != AL_PLAYING)
	{
		alSourcePlay(m_sources[sourceName]);
	}*/		
	if (bufferName == ThemeSong0)
	{
		m_error = alGetError();
		alSourcePlay(themeSong0Source);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error playing sound");
		}
	}
	else if (bufferName == PickupSpawnSound)
	{
		m_error = alGetError();
		alSourcePlay(pickupSpawnSource);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error playing sound");
		}
	}
	else if (bufferName == HitmarkSound)
	{
		m_error = alGetError();
		alSourcePlay(hitmarkSource);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error playing sound");
		}
	}
	else
	{
		bool found = false;
		for (int i = 0; i < m_nrOfPlayers && !found; i++)
		{
			if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
			{
 				if (bufferName == JumpSound || bufferName == BasicAttackSound ||
					bufferName == EnhanceAttackSound|| bufferName == TakingDamageSound)
				{
					m_error = alGetError();
					alSourcePlay(m_playerSoundInfo.at(i).sources.at(bufferName));

					if ((m_error = alGetError()) != AL_NO_ERROR)
					{
						logTrace("Error playing sound");
					}
				}
				else if (getSourceState(bufferName, playerID) != AL_PLAYING)
				{
					m_error = alGetError();
					alSourcePlay(m_playerSoundInfo.at(i).sources.at(bufferName));

					if ((m_error = alGetError()) != AL_NO_ERROR)
					{
						logTrace("Error playing sound");
					}
				}
				found = true;
			}
		}
	}
}

void SoundHandler::pauseSound(SoundIndex bufferName, RakNet::AddressOrGUID playerID)
{	
	if (bufferName == ThemeSong0)
	{
		m_error = alGetError();

		if (getSourceState(bufferName, playerID) == AL_PLAYING)
		{
			alSourcePause(themeSong0Source);
		}

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error pausing sound");
		}
	}
	else if (bufferName == PickupSpawnSound)
	{
		m_error = alGetError();

		if (getSourceState(bufferName, playerID) == AL_PLAYING)
		{
			alSourcePause(pickupSpawnSource);
		}

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error pausing sound");
		}
	}
	else
	{
		bool found = false;
		for (int i = 0; i < m_nrOfPlayers && !found; i++)
		{
			if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
			{
				m_error = alGetError();

				if (getSourceState(bufferName, playerID) == AL_PLAYING)
				{
					alSourcePause(m_playerSoundInfo.at(i).sources[bufferName]);
				}

				if ((m_error = alGetError()) != AL_NO_ERROR)
				{
					logTrace("Error pausing sound");
				}

				found = true;
			}
		}
	}
}

void SoundHandler::stopSound(SoundIndex bufferName, RakNet::AddressOrGUID playerID)
{
	if (bufferName == ThemeSong0)
	{
		m_error = alGetError();

		if (getSourceState(bufferName, playerID) == AL_PLAYING)
		{
			alSourceStop(themeSong0Source);
		}

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error stopping sound");
		}
	}
	else if (bufferName == PickupSpawnSound)
	{
		m_error = alGetError();

		if (getSourceState(bufferName, playerID) == AL_PLAYING)
		{
			alSourceStop(pickupSpawnSource);
		}

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error stopping sound");
		}
	}
	else
	{
		bool found = false;
		for (int i = 0; i < m_nrOfPlayers && !found; i++)
		{
			if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
			{
				m_error = alGetError();

				if (getSourceState(bufferName, playerID) == AL_PLAYING)
				{
					alSourceStop(m_playerSoundInfo.at(i).sources.at(bufferName));
				}

				if ((m_error = alGetError()) != AL_NO_ERROR)
				{
					logTrace("Error stopping sound");
				}

				found = true;
			}
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

void SoundHandler::setSourcePitch(float pitch, SoundIndex bufferName, RakNet::AddressOrGUID playerID)
{
	if (bufferName == ThemeSong0)
	{
		m_error = alGetError();

		alSourcef(themeSong0Source, AL_PITCH, pitch);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting pitch to source");
		}
	}
	else if (bufferName == PickupSpawnSound)
	{
		m_error = alGetError();

		alSourcef(pickupSpawnSource, AL_PITCH, pitch);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting pitch to source");
		}
	}
	else
	{
		bool found = false;
		for (int i = 0; i < m_nrOfPlayers && !found; i++)
		{
			if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
			{
				m_error = alGetError();

				alSourcef(m_playerSoundInfo.at(i).sources.at(bufferName), AL_PITCH, pitch);

				if ((m_error = alGetError()) != AL_NO_ERROR)
				{
					logTrace("Error setting pitch to source");
				}
				found = true;
			}
		}
	}
}

void SoundHandler::setSourceGain(float gain, SoundIndex bufferName, RakNet::AddressOrGUID playerID)
{
	if (bufferName == ThemeSong0)
	{
		m_error = alGetError();

		alSourcef(themeSong0Source, AL_GAIN, gain);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting gain to source");
		}
	}
	else if (bufferName == PickupSpawnSound)
	{
		m_error = alGetError();

		alSourcef(pickupSpawnSource, AL_GAIN, gain);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting gain to source");
		}
	}
	else if (bufferName == HitmarkSound)
	{
		m_error = alGetError();

		alSourcef(hitmarkSource, AL_GAIN, gain);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting gain to source");
		}
	}
	else
	{
		bool found = false;
		for (int i = 0; i < m_nrOfPlayers && !found; i++)
		{
			if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
			{
				m_error = alGetError();

				alSourcef(m_playerSoundInfo.at(i).sources.at(bufferName), AL_GAIN, gain);

				if ((m_error = alGetError()) != AL_NO_ERROR)
				{
					logTrace("Error setting gain to source");
				}
				found = true;
			}
		}
	}
}

//Don't use this for now
void SoundHandler::setSourceMaxDistance(float dist, SoundIndex bufferName, RakNet::AddressOrGUID playerID)
{	
	if (bufferName == ThemeSong0)
	{
		m_error = alGetError();

		alSourcef(themeSong0Source, AL_MAX_DISTANCE, dist);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting max distance to source");
		}
	}
	else if (bufferName == PickupSpawnSound)
	{
		m_error = alGetError();

		alSourcef(pickupSpawnSource, AL_MAX_DISTANCE, dist);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting max distance to source");
		}
	}
	else if (bufferName == HitmarkSound)
	{
		m_error = alGetError();

		alSourcef(hitmarkSource, AL_MAX_DISTANCE, dist);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting max distance to source");
		}
	}
	else
	{
		bool found = false;
		for (int i = 0; i < m_nrOfPlayers && !found; i++)
		{
			if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
			{
				m_error = alGetError();

				alSourcef(m_playerSoundInfo.at(i).sources.at(bufferName), AL_MAX_DISTANCE, dist);

				if ((m_error = alGetError()) != AL_NO_ERROR)
				{
					logTrace("Error setting max distance to source");
				}

				found = true;
			}
		}
	}
}

void SoundHandler::setSourcePosition(glm::vec3 pos, SoundIndex bufferName, RakNet::AddressOrGUID playerID)
{	
	ALfloat sourcePosition[] = { pos.x, pos.y, pos.z };

	if (bufferName == ThemeSong0)
	{
		m_error = alGetError();

		alSourcefv(themeSong0Source, AL_POSITION, sourcePosition);
		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting position to source");
		}
	}
	else if (bufferName == PickupSpawnSound)
	{
		m_error = alGetError();

		alSourcefv(pickupSpawnSource, AL_POSITION, sourcePosition);
		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting position to source");
		}
	}
	else if (bufferName == HitmarkSound)
	{
		m_error = alGetError();

		alSourcefv(hitmarkSource, AL_POSITION, sourcePosition);
		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting position to source");
		}
	}
	else
	{
		bool found = false;
		for (int i = 0; i < m_nrOfPlayers && !found; i++)
		{
			if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
			{
				m_error = alGetError();

				alSourcefv(m_playerSoundInfo.at(i).sources.at(bufferName), AL_POSITION, sourcePosition);
				if ((m_error = alGetError()) != AL_NO_ERROR)
				{
					logTrace("Error setting position to source");
				}
				found = true;
			}
		}
	}
}

void SoundHandler::setSourceVelocity(glm::vec3 vel, SoundIndex bufferName, RakNet::AddressOrGUID playerID)
{	
	ALfloat sourceVelocity[] = { vel.x, vel.y, vel.z };	

	if (bufferName == ThemeSong0)
	{
		m_error = alGetError();

		alSourcefv(themeSong0Source, AL_VELOCITY, sourceVelocity);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting velocity to source");
		}
	}
	else if (bufferName == PickupSpawnSound)
	{
		m_error = alGetError();

		alSourcefv(pickupSpawnSource, AL_VELOCITY, sourceVelocity);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting velocity to source");
		}
	}
	else if (bufferName == HitmarkSound)
	{
		m_error = alGetError();

		alSourcefv(hitmarkSource, AL_VELOCITY, sourceVelocity);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting velocity to source");
		}
	}
	else
	{
		bool found = false;
		for (int i = 0; i < m_nrOfPlayers && !found; i++)
		{
			if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
			{
				m_error = alGetError();

				alSourcefv(m_playerSoundInfo.at(i).sources.at(bufferName), AL_VELOCITY, sourceVelocity);

				if ((m_error = alGetError()) != AL_NO_ERROR)
				{
					logTrace("Error setting velocity to source");
				}
				found = true;
			}
		}
	}
}

void SoundHandler::setSourceDirection(glm::vec3 dir, SoundIndex bufferName, RakNet::AddressOrGUID playerID)
{	
	ALfloat sourceDirection[] = { dir.x, dir.y, dir.z };	

	if (bufferName == ThemeSong0)
	{
		m_error = alGetError();

		alSourcefv(themeSong0Source, AL_DIRECTION, sourceDirection);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting direction to source");
		}
	}
	else if (bufferName == PickupSpawnSound)
	{
		m_error = alGetError();

		alSourcefv(pickupSpawnSource, AL_DIRECTION, sourceDirection);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting direction to source");
		}
	}
	else if (bufferName == HitmarkSound)
	{
		m_error = alGetError();

		alSourcefv(hitmarkSource, AL_DIRECTION, sourceDirection);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting direction to source");
		}
	}
	else
	{
		bool found = false;
		for (int i = 0; i < m_nrOfPlayers && !found; i++)
		{
			if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
			{
				m_error = alGetError();

				alSourcefv(m_playerSoundInfo.at(i).sources.at(bufferName), AL_DIRECTION, sourceDirection);

				if ((m_error = alGetError()) != AL_NO_ERROR)
				{
					logTrace("Error setting direction to source");
				}
				found = true;
			}
		}
	}
}

//Types are: AL_UNDETERMINED, AL_STATIC or AL_STREAMING
void SoundHandler::setSourceType(ALenum type, SoundIndex bufferName, RakNet::AddressOrGUID playerID)
{
	if (bufferName == ThemeSong0)
	{
		m_error = alGetError();
		alSourcei(themeSong0Source, AL_SOURCE_TYPE, type);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting type to source");
		}
	}
	else if (bufferName == PickupSpawnSound)
	{
		m_error = alGetError();
		alSourcei(pickupSpawnSource, AL_SOURCE_TYPE, type);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting type to source");
		}
	}
	else if (bufferName == HitmarkSound)
	{
		m_error = alGetError();
		alSourcei(hitmarkSource, AL_SOURCE_TYPE, type);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting type to source");
		}
	}
	else
	{
		bool found = false;
		for (int i = 0; i < m_nrOfPlayers && !found; i++)
		{
			if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
			{
				m_error = alGetError();
				alSourcei(m_playerSoundInfo.at(i).sources.at(bufferName), AL_SOURCE_TYPE, type);

				if ((m_error = alGetError()) != AL_NO_ERROR)
				{
					logTrace("Error setting type to source");
				}
				found = true;
			}
		}
	}
}

void SoundHandler::setSourceLooping(bool looping, SoundIndex bufferName, RakNet::AddressOrGUID playerID)
{	
	if (bufferName == ThemeSong0)
	{
		m_error = alGetError();

		alSourcei(themeSong0Source, AL_LOOPING, looping);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting looping value to source");
		}
	}
	else if (bufferName == PickupSpawnSound)
	{
		m_error = alGetError();

		alSourcei(pickupSpawnSource, AL_LOOPING, looping);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting looping value to source");
		}
	}
	else if (bufferName == HitmarkSound)
	{
		m_error = alGetError();

		alSourcei(hitmarkSource, AL_LOOPING, looping);

		if ((m_error = alGetError()) != AL_NO_ERROR)
		{
			logTrace("Error setting looping value to source");
		}
	}
	else
	{
		bool found = false;
		for (int i = 0; i < m_nrOfPlayers && !found; i++)
		{
			if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
			{
				m_error = alGetError();

				alSourcei(m_playerSoundInfo.at(i).sources.at(bufferName), AL_LOOPING, looping);

				if ((m_error = alGetError()) != AL_NO_ERROR)
				{
					logTrace("Error setting looping value to source");
				}
				found = true;
			}
		}
	}
}

void SoundHandler::setPlayerGUIDs()
{	
	auto& list = Client::getInstance()->getConnectedPlayers();
	PlayerPacket myDataPacket = Client::getInstance()->getMyData();
	
	if(m_playerSoundInfo.size() < 1)
			m_playerSoundInfo.resize(1);

	m_playerSoundInfo.at(0).guid = myDataPacket.guid;

	for (int i = 1; i < list.size() + 1; i++)
	{
		if (m_playerSoundInfo.size() < i + 1)
			m_playerSoundInfo.resize(m_playerSoundInfo.size() + 1);
		m_playerSoundInfo[i].guid = list[i].guid;
	}	

	m_nrOfPlayers = list.size() + 1;
}

void SoundHandler::addPlayer(RakNet::AddressOrGUID guid)
{
	m_playerSoundInfo.resize(m_playerSoundInfo.size() + 1);
	m_playerSoundInfo.at(m_playerSoundInfo.size() - 1).guid = guid;
	m_nrOfPlayers++;
	attachBuffersToPlayerSources(guid);
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
				alSourcei(m_playerSoundInfo.at(i).sources.at(j), AL_BUFFER, NULL);
				alDeleteSources(1, &m_playerSoundInfo.at(i).sources.at(j));
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

const ALint& SoundHandler::getSourceState(SoundIndex bufferName, RakNet::AddressOrGUID playerID) const
{
	ALint value;
	
	if (bufferName == ThemeSong0)
	{
		alGetSourcei(themeSong0Source, AL_SOURCE_STATE, &value);
	}
	else if (bufferName == PickupSpawnSound)
	{
		alGetSourcei(pickupSpawnSource, AL_SOURCE_STATE, &value);
	}
	else if (bufferName == HitmarkSound)
	{
		alGetSourcei(hitmarkSource, AL_SOURCE_STATE, &value);
	}
	else
	{	
		bool found = false;
		for (int i = 0; i < m_playerSoundInfo.size() && !found; i++)
		{
			if (m_playerSoundInfo.at(i).guid.rakNetGuid == playerID.rakNetGuid)
			{
				alGetSourcei(m_playerSoundInfo.at(i).sources.at(bufferName), AL_SOURCE_STATE, &value);
				found = true;
			}
		}
	}	

	return value;
}