#include "Pch/Pch.h"
#include "SoundHandler.h"

SoundHandler::SoundHandler()
{
	ALCenum error;
	alutInitWithoutContext(NULL, NULL);

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

	error = alGetError();

	if (error != AL_NO_ERROR)
	{		
		logTrace(alutGetErrorString(error));
	}



	error = alGetError();

	m_buffers.resize(m_buffers.size() + 1);

	alGenBuffers((ALsizei)1, &m_buffers[0]);

	error = alGetError();

	if (error != AL_NO_ERROR)
	{
		logTrace("Error generating buffers");
		logTrace(alutGetErrorString(error));
	}
		
	loadSound("YouShallNotPass.wav");

	error = alGetError();

	m_sources.resize(m_sources.size() + 1);

	alGenSources((ALsizei)1, &m_sources[0]);

	error = alGetError();

	if (error != AL_NO_ERROR)
	{
		logTrace("Error generating sources");
		logTrace(alutGetErrorString(error));
	}

	error = alGetError();

	alSourcei(m_sources[0], AL_BUFFER, m_buffers[0]);

	error = alGetError();

	if (error != AL_NO_ERROR)
	{
		logTrace("Error binding buffer to source");
		logTrace(alutGetErrorString(error));
	}
}

void SoundHandler::loadSound(const char* filename)
{	
	/*ALsizei size, freq;
	ALenum format;
	ALboolean loop = AL_FALSE;
	void* data;*/

	//alutLoadWAVFile((ALbyte*)&filename, &format, &data, &size, &freq, &loop);
	m_buffers[0] = alutCreateBufferFromFile(filename);	

	//alBufferData(m_buffers[0], format, data, size, freq);
}

void SoundHandler::playSound(int bufferName)
{
	ALCenum error;

	error = alGetError();

	alSourcePlay(m_sources[bufferName]);

	if (error != AL_NO_ERROR)
	{
		logTrace(alutGetErrorString(error));
	}
}

SoundHandler::~SoundHandler()
{
	delete m_device;
}
