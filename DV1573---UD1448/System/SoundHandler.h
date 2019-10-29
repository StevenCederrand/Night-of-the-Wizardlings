#ifndef SOUNDHANDLER_H
#define SOUNDHANDLER_H
#include <OpenAl/al.h>
#include <OpenAL/alc.h>
#include <OpenAL/AL/alut.h>

class SoundHandler
{
private:
	ALCdevice* m_device;
	ALuint* m_buffers;
public:
	SoundHandler();

	const ALvoid* loadSound(const char* filename);

	~SoundHandler();

};

#endif
