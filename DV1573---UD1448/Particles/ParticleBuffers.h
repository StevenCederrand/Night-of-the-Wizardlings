#ifndef P_BUFFERS_H
#define P_BUFFERS_H

#include "Pch/Pch.h"
#include "Particles.h"
#include <random>

class ParticleBuffers
{
private:
	//unsigned int or GLuint?
	unsigned int m_texture;
	unsigned int m_vao;
	unsigned int m_vertexBuffer;
	unsigned int m_directionalBuffer;
	unsigned int m_lifetimeBuffer;
	unsigned int m_shader;
	unsigned int m_vertexPosition;
	unsigned int m_vertexDirection;
	unsigned int m_vertexLife;
	//unsigned int m_texture;

	TextureInfo m_txtInfo;
	PSinfo m_psInfo;

	//Do I really need everything here??
	std::vector<glm::vec3> m_vertex;
	std::vector<glm::vec3> m_directionVector;

	std::vector<Particle> m_particle;
	std::vector<float> m_lifetime;

	//psBuffers psBuffer;

public:
	ParticleBuffers();
	ParticleBuffers(PSinfo psInfo, TextureInfo txtInfo);
	~ParticleBuffers();

	//Do I need to set more things here?
	void setTexture(TextureInfo txtInfo); //Might not need?
	void setShader(unsigned int shader);

	void bindBuffers();

	//Getters which are needed in the particle system
	unsigned int getVAO();
	unsigned int getTexture();
	unsigned int getVertexBuffer();
	unsigned int getDirectionalBuffer();
	unsigned int getLifetimeBuffer();
	unsigned int& getShader();
	unsigned int& getVertexPos();
	unsigned int& getVertexDir();
	unsigned int& getVertexLife();

	std::vector<glm::vec3> getVertex();
	std::vector<glm::vec3> getDir();
	std::vector<Particle> getParticle();
	std::vector<float> getLifetime();

	PSinfo getPSinfo();
	TextureInfo getTxtInfo();
};



#endif