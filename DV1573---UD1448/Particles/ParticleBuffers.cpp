#include "Pch/Pch.h"
#include "ParticleBuffers.h"

ParticleBuffers::ParticleBuffers(PSinfo psInfo)
{
	//No way this is gonna work

	m_texture = 0;
	m_vao = 0;
	m_vertexBuffer = 0;
	m_directionalBuffer = 0;
	m_lifetimeBuffer = 0;
	m_shader = 0;
	m_vertexPosition = 0;
	m_vertexDirection = 0;
	m_vertexLife = 0;
	m_psInfo = psInfo;
}

ParticleBuffers::~ParticleBuffers()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteTextures(1, &m_texture);
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_lifetimeBuffer);
	glDeleteBuffers(1, &m_directionalBuffer);
}

void ParticleBuffers::setTexture(TextureInfo txtInfo)
{
	m_txtInfo = txtInfo;
}

void ParticleBuffers::setShader(unsigned int shader)
{
	m_shader = shader;
}

void ParticleBuffers::bindBuffers()
{
	std::mt19937 gen(m_psInfo.seed);
	std::uniform_real_distribution<double> dis(-1.0f, 1.0f);
	for (int i = 0; i < m_psInfo.maxParticles; i++)
	{
		float x = dis(gen);
		float y = dis(gen);
		float z = dis(gen);

		Particle particles;
		glm::vec3 v = glm::vec3(0.0f, 0.0f, 0.0f);

		particles.position = glm::vec3(0.0f, 0.0f, 0.0f);
		particles.distance = -1.0f;
		particles.isAlive = false;
		particles.startLoop = true;
		particles.rotation = glm::vec3(x, y, z);
		particles.time = m_psInfo.lifetime;
		particles.velocity = glm::vec3(0.0f, 0.0f, 0.0f);

		//Pushbacks
		m_vertex.push_back(v);
		m_particle.push_back(particles);
		m_directionVector.push_back(particles.rotation);
		m_lifetime.push_back(particles.time);
	}


	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	unsigned char* imageData = stbi_load(m_txtInfo.name.c_str(), &m_txtInfo.width, &m_txtInfo.height, &m_txtInfo.nrChannels, 0); //String to const char, might need a check if problemo
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_txtInfo.width, m_txtInfo.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	stbi_image_free(imageData);
	glUniform1i(glGetUniformLocation(m_shader, "ps_texture"), 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//gen vao
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	//bind vao
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertex.size() * sizeof(glm::vec3), &m_vertex[0], GL_STATIC_DRAW);
	m_vertexPosition = glGetAttribLocation(m_shader, "vtxPos");
	glGenBuffers(1, &m_directionalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_directionalBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_directionVector.size() * sizeof(glm::vec3), &m_directionVector[0], GL_STATIC_DRAW);
	m_vertexDirection = glGetAttribLocation(m_shader, "vtxDir");
	glGenBuffers(1, &m_lifetimeBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_lifetimeBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_lifetime.size() * sizeof(float), &m_lifetime[0], GL_STATIC_DRAW);
	m_vertexLife = glGetAttribLocation(m_shader, "vtxLifetime");
	//unbind vao
	glBindVertexArray(0);
}

unsigned int ParticleBuffers::getVAO()
{
	return m_vao;
}

unsigned int ParticleBuffers::getTexture()
{
	return m_texture;
}

unsigned int ParticleBuffers::getVertexBuffer()
{
	return m_vertexBuffer;
}

unsigned int ParticleBuffers::getDirectionalBuffer()
{
	return m_directionalBuffer;
}

unsigned int ParticleBuffers::getLifetimeBuffer()
{
	return m_lifetimeBuffer;
}

unsigned int& ParticleBuffers::getShader()
{
	return m_shader;
}

unsigned int& ParticleBuffers::getVertexPos()
{
	return m_vertexPosition;
}

unsigned int& ParticleBuffers::getVertexDir()
{
	return m_vertexDirection;
}

unsigned int& ParticleBuffers::getVertexLife()
{
	return m_vertexLife;
}

std::vector<glm::vec3> ParticleBuffers::getVertex()
{
	return m_vertex;
}

std::vector<glm::vec3> ParticleBuffers::getDir()
{
	return m_directionVector;
}

std::vector<Particle> ParticleBuffers::getParticle()
{
	return m_particle;
}

std::vector<float> ParticleBuffers::getLifetime()
{
	return m_lifetime;
}

psBuffers ParticleBuffers::getBuffer()
{
	psBuffer.texture = m_texture;
	psBuffer.vao = m_vao;
	psBuffer.vertexBuffer = m_vertexBuffer;
	psBuffer.directionalBuffer = m_directionalBuffer;
	psBuffer.lifetimeBuffer = m_lifetimeBuffer;
	psBuffer.shader = m_shader;
	psBuffer.vertexPosition = m_vertexPosition;
	psBuffer.vertexDirection = m_vertexDirection;
	psBuffer.vertexLife = m_vertexLife;

	return psBuffer;
}