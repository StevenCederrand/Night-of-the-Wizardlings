#include "Pch/Pch.h"
#include "SoundHandler.h"
#include "vorbis/vorbisfile.h"

SoundHandler* SoundHandler::m_soundHandlerInstance = 0;

SoundHandler::SoundHandler()
{
	m_buffers.resize(NR_OF_SOUNDS);
	m_sources.resize(NR_OF_SOUNDS);

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
}

SoundHandler::~SoundHandler()
{
	for (int i = 0; i < m_sources.size(); i++)
	{
		alSourcei(m_sources[i], AL_BUFFER, NULL);
	}
	
	for (int i = 0; i < m_sources.size(); i++)
	{
		alDeleteSources(1, &m_sources[i]);
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

	success = loadSound(BasicAttackSoundIndex);
	if (success == -1)
	{
		logTrace(BASIC_ATTACK_SOUND + " failed to be loaded");
	}

	success = loadSound(DeflectSoundIndex);
	if (success == -1)
	{
		logTrace(DEFLECT_SOUND + " failed to be loaded");
	}

	success = loadSound(EnhanceAttackSoundIndex);
	if (success == -1)
	{
		logTrace(ENHANCE_ATTACK_SOUND + " failed to be loaded");
	}	
}

//Returns the source name (position in the buffer array) which you use 
//when you call playSound(int sourceName).
//Returns -1 if failed
int SoundHandler::loadSound(SoundIndex whatSound)
{
	std::string fileName = SOUNDEFFECTPATH;

	switch (whatSound)
	{
	case ThemeSong0:
		fileName += THEME_SONG0;
		break;
	case BasicAttackSoundIndex:
		fileName += BASIC_ATTACK_SOUND;
		break;
	case DeflectSoundIndex:
		fileName += DEFLECT_SOUND;
		break;
	case EnhanceAttackSoundIndex:
		fileName += ENHANCE_ATTACK_SOUND;
		break;
	case TakingDamageSoundIndex:
		break;
	case PickupSpawnSoundIndex:
		break;
	case StepsSoundIndex:
		break;
	}

	ALenum error = 0;
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
	error = alGetError();

	alGenBuffers((ALsizei)1, &m_buffers[whatSound]);

	error = alGetError();

	if (error != AL_NO_ERROR)
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

	error = alGetError();
	//send data to openal, vInfo->rate is your freq in Hz
	alBufferData(m_buffers[whatSound], format, pcmout, data_len, vInfo->rate);
	if ((error = alGetError() != AL_NO_ERROR))
	{
		logTrace("Failed to send audio information buffer to OpenAL");

		free(pcmout);
		fclose(fp);
		ov_clear(&vf);
		return -1;
	}

	error = alGetError();

	alGenSources((ALsizei)1, &m_sources[whatSound]);

	error = alGetError();

	if (error != AL_NO_ERROR)
	{
		logTrace("Error generating sources");		

		free(pcmout);
		fclose(fp);
		ov_clear(&vf);
		return -1;
	}

	error = alGetError();

	alSourcei(m_sources[whatSound], AL_BUFFER, m_buffers[whatSound]);

	error = alGetError();

	if (error != AL_NO_ERROR)
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

void SoundHandler::playSound(SoundIndex sourceName)
{
	m_error = alGetError();

	/*if (getSourceState(sourceName) != AL_PLAYING)
	{
		alSourcePlay(m_sources[sourceName]);
	}*/

	alSourcePlay(m_sources[sourceName]);

	if (m_error != AL_NO_ERROR)
	{
		logTrace("Error playing sound");
	}
}

void SoundHandler::pauseSound(SoundIndex sourceName)
{
	m_error = alGetError();

	if (getSourceState(sourceName) == AL_PLAYING)
	{
		alSourcePause(m_sources[sourceName]);
	}

	if (m_error != AL_NO_ERROR)
	{
		logTrace("Error pausing sound");
	}	
}

void SoundHandler::stopSound(SoundIndex sourceName)
{
	m_error = alGetError();

	if (getSourceState(sourceName) == AL_PLAYING)
	{
		alSourceStop(m_sources[sourceName]);
	}

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error stopping sound");
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

void SoundHandler::setSourcePitch(float pitch, SoundIndex sourceName)
{
	m_error = alGetError();

	alSourcef(m_sources[sourceName], AL_PITCH, pitch);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting pitch to source");
	}
}

void SoundHandler::setSourceGain(float gain, SoundIndex sourceName)
{
	m_error = alGetError();

	alSourcef(m_sources[sourceName], AL_GAIN, gain);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting gain to source");
	}
}

void SoundHandler::setSourceMaxDistance(float dist, SoundIndex sourceName)
{	
	m_error = alGetError();

	alSourcef(m_sources[sourceName], AL_MAX_DISTANCE, dist);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting max distance to source");
	}
}

void SoundHandler::setSourcePosition(glm::vec3 pos, SoundIndex sourceName)
{	
	ALfloat sourcePosition[] = { pos.x, pos.y, pos.z };

	m_error = alGetError();

	alSourcefv(m_sources[sourceName], AL_POSITION, sourcePosition);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting position to source");
	}
}

void SoundHandler::setSourceVelocity(glm::vec3 vel, SoundIndex sourceName)
{	
	ALfloat sourceVelocity[] = { vel.x, vel.y, vel.z };

	m_error = alGetError();

	alSourcefv(m_sources[sourceName], AL_VELOCITY, sourceVelocity);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting velocity to source");
	}
}

void SoundHandler::setSourceDirection(glm::vec3 dir, SoundIndex sourceName)
{	
	ALfloat sourceDirection[] = { dir.x, dir.y, dir.z };

	m_error = alGetError();

	alSourcefv(m_sources[sourceName], AL_DIRECTION, sourceDirection);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting direction to source");
	}
}

//Types are: AL_UNDETERMINED, AL_STATIC or AL_STREAMING
void SoundHandler::setSourceType(ALenum type, SoundIndex sourceName)
{	
	m_error = alGetError();

	alSourcei(m_sources[sourceName], AL_SOURCE_TYPE, type);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting type to source");
	}
}

void SoundHandler::setSourceLooping(bool looping, SoundIndex sourceName)
{	
	m_error = alGetError();

	alSourcei(m_sources[sourceName], AL_LOOPING, looping);

	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		logTrace("Error setting looping value to source");
	}
}

const ALint& SoundHandler::getSourceState(SoundIndex sourceName) const
{
	ALint value;

	alGetSourcei(m_sources[sourceName], AL_SOURCE_STATE, &value);

	return value;
}