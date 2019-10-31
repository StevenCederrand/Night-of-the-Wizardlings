#ifndef SOUNDHANDLER_H
#define SOUNDHANDLER_H
#include <OpenAl/al.h>
#include <OpenAL/alc.h>
#include <OpenAL/AL/alut.h>

struct BufferData
{
	ALsizei size;
	ALsizei freq;
	ALenum format;
	ALvoid* data;
	ALboolean loop;
};

class SoundHandler
{
private:
	ALCdevice* m_device;
	std::vector<ALuint> m_buffers;
	std::vector<BufferData> m_bData;
public:
	SoundHandler();

	void loadSound(const char* filename);

	~SoundHandler();

};

#endif
