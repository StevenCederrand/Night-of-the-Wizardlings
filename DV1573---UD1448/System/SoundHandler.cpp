#include "Pch/Pch.h"
#include "SoundHandler.h"

SoundHandler::SoundHandler()
{
	ALCenum error;

	error = alGetError();

	alGenBuffers(1, &m_buffers[0]);

	error = alGetError();

	if (error != AL_NO_ERROR)
	{
		logTrace("Error generating buffers");
	}
	
	loadSound("Test.wav");

	alBufferData(m_buffers[0],
		AL_FORMAT_MONO16,
		m_bData[0].data,
		m_bData[0].size,
		m_bData[0].freq);


}

void SoundHandler::loadSound(const char* filename)
{
	//TODO: save in vector of a struct maybe, containing all the necesarry values
	//for the bufferdata. 	

	/*m_bData[0].loop = AL_FALSE;

	ALsizei size;
	ALsizei freq;
	ALenum format;
	ALvoid* data;
	ALboolean loop = AL_FALSE;*/

	/*alutLoadWAVFile((ALbyte*)filename,
		&m_bData[0].format,
		&m_bData[0].data, &m_bData[0].size,
		&m_bData[0].freq, &m_bData[0].loop);*/
	
	m_buffers[m_buffers.size()] = alutCreateBufferFromFile(filename);
	
}

SoundHandler::~SoundHandler()
{
	for (int i = 0; i < m_bData.size(); i++)
	{
		delete m_bData[i].data;
	}

	delete m_device;
}
