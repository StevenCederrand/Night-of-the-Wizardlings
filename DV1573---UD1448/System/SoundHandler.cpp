#include "Pch/Pch.h"
#include "SoundHandler.h"

SoundHandler::SoundHandler()
{
	ALCenum error;

	error = alGetError();

	alGenBuffers(1, m_buffers);

	error = alGetError();

	if (error != AL_NO_ERROR)
	{
		logTrace("Error generating buffers");
	}
	
	const ALvoid* data = loadSound("Test.wav");

	alBufferData(m_buffers[0], AL_FORMAT_MONO16, data, );

}

void SoundHandler::loadSound(ALuint& bufferName)
{
	//TODO: save in vector of a struct maybe, containing all the necesarry values
	//for the bufferdata. 
	ALsizei m_size, m_freq;
	ALenum m_format;
	ALvoid* m_data;
	ALboolean m_loop = AL_FALSE;

	alutLoadWAVFile((ALbyte*)filename, &format, &data, &size, &freq, &loop);

	return data;
}

SoundHandler::~SoundHandler()
{
}
