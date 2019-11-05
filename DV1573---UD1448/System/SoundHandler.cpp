#include "Pch/Pch.h"
#include "SoundHandler.h"
#include "vorbis/vorbisfile.h"

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

	/*error = alGetError();

	m_buffers.resize(m_buffers.size() + 1);

	alGenBuffers((ALsizei)1, &m_buffers[0]);

	error = alGetError();

	if (error != AL_NO_ERROR)
	{
		logTrace("Error generating buffers");
		logTrace(alutGetErrorString(error));
	}*/
		
	loadSound("Assets/SoundEffects/YouShallNotPassogg.ogg");

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
	/*ALsizei size;
	ALfloat freq;
	ALenum format;
	ALboolean loop = AL_FALSE;
	void* data;
	ALCenum error;*/


	//alutLoadWAVFile((ALbyte*)&filename, &format, &data, &size, &freq, &loop);
	//m_buffers[0] = alutCreateBufferFromFile(filename);	

	/*error = alGetError();
	data = alutLoadMemoryFromFile(filename, &format, &size, &freq);
	error = alGetError();

	

	if (error != AL_NO_ERROR)
	{
		logTrace(alutGetErrorString(error));
	}

	alBufferData(m_buffers[0], format, data, size, freq);*/

	ALenum error = 0;
	ALuint* sound = 0;
	FILE* fp = 0;
	OggVorbis_File vf;
	vorbis_info* vInfo;
	ALenum format = 0;
	short* pcmout = 0;

	fopen_s(&fp, filename, "rb");

	if (fp == 0)
	{
		logTrace("Could not open sound file");
	}

	//Generate buffer
	error = alGetError();

	m_buffers.resize(m_buffers.size() + 1);

	alGenBuffers((ALsizei)1, &m_buffers[0]);

	error = alGetError();

	if (error != AL_NO_ERROR)
	{
		logTrace("Error generating buffers");
		logTrace(alutGetErrorString(error));
	}

	//Open ogg file
	if (ov_open_callbacks(fp, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)
	{
		logTrace("Stream is not a valid OggVorbis stream!");
	}

	//Fill vInfo with a new ogg vorbis info struct, determine audio format
	vInfo = ov_info(&vf, -1);
	format = vInfo->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	size_t data_len = ov_pcm_total(&vf, -1) * vInfo->channels * 2; // 16bits
	pcmout = (short*)malloc(data_len);
	if (pcmout == 0)
	{
		logTrace("Out of memory");
	}

	//Fill pcmout buffer with ov_read data samples
	for (size_t size = 0, offset = 0, sel = 0;
		(size = ov_read(&vf, (char*)pcmout + offset, 4096, 0, 2, 1, (int*)& sel)) != 0;
		offset += size)
	{
		if (size < 0)
		{
			logTrace("Something wrong with the ogg file");
		}
	}

	error = alGetError();
	//send data to openal, vInfo->rate is your freq in Hz
	alBufferData(m_buffers[0], format, pcmout, data_len, vInfo->rate);
	if ((error = alGetError() != AL_NO_ERROR))
	{
		logTrace("Failed to send audio information buffer to OpenAL");
	}

	free(pcmout);
	fclose(fp);
	ov_clear(&vf);
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
