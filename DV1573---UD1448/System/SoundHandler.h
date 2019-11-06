#ifndef SOUNDHANDLER_H
#define SOUNDHANDLER_H
#include <OpenAl/al.h>
#include <OpenAL/alc.h>

class SoundHandler
{
private:
	ALCdevice* m_device;
	ALCcontext* m_context;
	std::vector<ALuint> m_buffers;	
	std::vector<ALuint> m_sources;
	ALenum m_error;
public:
	SoundHandler();
	~SoundHandler();

	int loadSound(const char* filename);
	void playSound(int sourceName = 0);
	void pauseSound(int sourceName = 0);
	void stopSound(int sourceName = 0);

	void setListenerPos(glm::vec3 pos);
	void setListenerVelocity(glm::vec3 vel);
	void setListenerOrientation(glm::vec3 lookAt, glm::vec3 up);

	void setSourcePitch(float pitch, int sourceName = 0);
	void setSourceGain(float gain, int sourceName = 0);
	void setSourceMaxDistance(float dist, int sourceName = 0);
	void setSourcePosition(glm::vec3 pos, int sourceName = 0);
	void setSourceVelocity(glm::vec3 vel, int sourceName = 0);
	void setSourceDirection(glm::vec3 dir, int sourceName = 0);
	void setSourceType(ALenum type, int sourceName = 0); //AL_UNDETERMINED, AL_STATIC or AL_STREAMING
	void setSourceLooping(bool looping, int sourceName = 0);

	const ALint& getSourceState(int sourceName = 0) const;
	//source relative?	
};

#endif
