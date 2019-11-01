#ifndef SOUNDHANDLER_H
#define SOUNDHANDLER_H
#include <OpenAl/al.h>
#include <OpenAL/alc.h>
#include <OpenAL/AL/alut.h>

class SoundHandler
{
private:
	ALCdevice* m_device;
	ALCcontext* m_context;
	std::vector<ALuint> m_buffers;	
	std::vector<ALuint> m_sources;
public:
	SoundHandler();
	
	void loadSound(const char* filename);
	void playSound(int bufferName);

	~SoundHandler();

};

#endif
