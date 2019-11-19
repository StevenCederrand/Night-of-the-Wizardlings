#ifndef PARTICLE_H
#define PARTICLE_H

#include <Pch/Pch.h>
#include <streambuf>

const int MaxParticles = 100000;


struct TextureInfo //Information for the texture that we apply to the particles
{
	int width;
	int height;
	std::string name;
	int nrChannels; //Do I actually need this?
};

struct Particle //Basic particle data
{
	float distance;
	float time;

	bool startLoop;
	bool isAlive;

	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 rotation;
	glm::vec3 velocity;
};

struct PSinfo //All the fun particle information values
{
	glm::vec3 direction;
	glm::vec3 color;
	glm::vec3 blendColor;

	int maxParticles;
	int cont;
	int omnious;
	int seed;
	int fade;
	int scaleDirection;
	int glow;
	int swirl;

	float width;
	float heigth;
	float lifetime;
	float emission;
	float spread;
	float force;
	float drag;
	float gravity;
};

class ParticleSystem
{
private:

	TextureInfo* m_txtInfo;
	PSinfo* m_psInfo;

	glm::vec3 m_position;
	glm::vec3 m_direction;

	glm::mat4 m_quad;
	glm::mat4 m_lookAt;

	std::vector<glm::vec3> m_vertex;
	std::vector<glm::vec3> m_directionVector;

	std::vector<Particle> m_particle;
	std::vector<float> m_lifetime;

	GLuint m_texture;
	GLuint m_vao;
	GLuint m_vertexBuffer;
	GLuint m_directionalBuffer;
	GLuint m_lifetimeBuffer;
	GLuint m_shader;
	GLuint m_vertexPosition;
	GLuint m_vertexDirection;
	GLuint m_vertexLife;

	bool m_isPlaying;
	bool m_cont;

	float m_current;

	int m_nrOfActive;


public:
	//unsigned int shader is not used!
	ParticleSystem(PSinfo* psInfo, TextureInfo* txtInfo, glm::vec3 position, GLuint shader, psBuffers psBuffer,
		std::vector<glm::vec3> vertex, std::vector<glm::vec3> directionVector, std::vector<Particle> particle, std::vector<float> lifetime);
	//ParticleSystem();
	~ParticleSystem();

	PSinfo* GetParticleInfo();
	TextureInfo* GetTextureInfo();

	glm::mat4 GetQuad();

	bool Initialize();
	bool Build(PSinfo* psInfo);
	bool Texture(TextureInfo* txtInfo);
	bool Plays();

	int GetNrOfParticles();

	void Update(PSinfo* psInfo, glm::vec3 cameraPos, float time);
	void TempInit(PSinfo* psInfo);
	void Render(const Camera* camera, const PSinfo* psInfo);
	void Pause();
	void Play();

	//void LoadInfo(unsigned int texture, unsigned int vao, unsigned int vertexBuffer, unsigned int directionalBuffer,
	//	unsigned int lifetimeBuffer, unsigned int vertexPos, unsigned int vertexDir, unsigned int vertexLife,
	//	std::vector<glm::vec3> vertex, std::vector<glm::vec3> directionVector, std::vector<Particle> particle, std::vector<float> lifetime);

	void LoadInfo(psBuffers psBuffer);

	void SetPosition(glm::vec3 pos);
};

#endif