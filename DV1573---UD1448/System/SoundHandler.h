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
	void playSound(int sourceName);
	void pauseSound(int sourceName);
	void stopSound(int sourceName);

	void setListenerPos(glm::vec3 pos);
	void setListenerVelocity(glm::vec3 vel);
	void setListenerOrientation(glm::vec3 lookAt, glm::vec3 up);

	void setSourcePitch(int sourceName, float pitch);
	void setSourceGain(int sourceName, float gain);
	void setSourceMaxDistance(int sourceName, float dist);
	void setSourcePosition(int sourceName, glm::vec3 pos);
	void setSourceVelocity(int sourceName, glm::vec3 vel);
	void setSourceDirection(int sourceName, glm::vec3 dir);
	void setSourceType(int sourceName, ALenum type); //AL_UNDETERMINED, AL_STATIC or AL_STREAMING
	void setSourceLooping(int sourceName, bool looping);	

	const ALint& getSourceState(int sourceName) const;
	//source relative?	
};

#endif
