#include "Pch/Pch.h"
#include "Particles.h"

#include <random>

ParticleSystem::ParticleSystem(ParticleBuffers* particleBuffers)
{
	m_txtInfo = particleBuffers->getTxtInfo();//&rings;
	m_psInfo = particleBuffers->getPSinfo();
	m_position = glm::vec3(0, -1000.0f, -20.0f);
	m_shader = particleBuffers->getShader();
	m_current = particleBuffers->getPSinfo().emission;

	//Instead of 0 this now gets data from the buffer class
	m_texture = particleBuffers->getTexture();
	m_vertexBuffer = particleBuffers->getVertexBuffer();
	m_directionalBuffer = particleBuffers->getDirectionalBuffer();
	m_lifetimeBuffer = particleBuffers->getLifetimeBuffer();
	m_vertexPosition = particleBuffers->getVertexPos();
	m_vertexDirection = particleBuffers->getVertexDir();
	m_vertexLife = particleBuffers->getVertexLife();

	//This is for the vector info
	m_vertex = particleBuffers->getVertex();
	m_directionVector = particleBuffers->getDir();
	m_particle = particleBuffers->getParticle();
	m_lifetime = particleBuffers->getLifetime();
	
	m_vao = particleBuffers->getVAO();

	Initialize();
}

ParticleSystem::~ParticleSystem()
{
	//m_psInfo = NULL;
	//m_txtInfo = NULL;
	//For GPU leaks
	//glDeleteVertexArrays(1, &m_vao);
	//glDeleteTextures(1, &m_texture);
	//glDeleteBuffers(1, &m_vertexBuffer);
	//glDeleteBuffers(1, &m_lifetimeBuffer);
	//glDeleteBuffers(1, &m_directionalBuffer);
	m_texture = 0;
	m_vertexBuffer = 0;
	m_directionalBuffer = 0;
	m_lifetimeBuffer = 0;
	m_vertexPosition = 0;
	m_vertexDirection = 0;
	m_vertexLife = 0;
	m_shader = 0;
}

PSinfo ParticleSystem::GetParticleInfo()
{
	return m_psInfo;
}

TextureInfo ParticleSystem::GetTextureInfo()
{
	return m_txtInfo;
}

glm::mat4 ParticleSystem::GetQuad()
{
	return m_quad;
}

bool ParticleSystem::Initialize()
{
	m_isPlaying = true;

	return true;
}

bool ParticleSystem::Build(PSinfo psInfo)
{
	m_psInfo = psInfo;
	m_vertex.resize(m_psInfo.maxParticles);
	m_particle.resize(m_psInfo.maxParticles);
	m_directionVector.resize(m_psInfo.maxParticles);
	m_lifetime.resize(m_psInfo.maxParticles);
	return true;
}

bool ParticleSystem::Texture(TextureInfo txtInfo)
{
	m_texture = 0;
	m_txtInfo = txtInfo;
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	unsigned char* imageData = stbi_load(m_txtInfo.name.c_str(), &m_txtInfo.width, &m_txtInfo.height, &m_txtInfo.nrChannels, 0); //String to const char, might need a check if problemo
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_txtInfo.width, m_txtInfo.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	stbi_image_free(imageData);
	glUniform1i(glGetUniformLocation(m_shader, "ps_texture"), 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return true;
}

bool ParticleSystem::Plays()
{
	return m_isPlaying;
}

int ParticleSystem::GetNrOfParticles()
{
	return m_nrOfActive;
}

void ParticleSystem::Update(float time) //removed campos
{
	if (m_isPlaying == true)
	{
		float deltaTime = time;
		m_nrOfActive = m_psInfo.maxParticles;
		//m_psInfo = psInfo;

		if (m_current > 0.0f)
		{
			m_current = (m_current - deltaTime);
		}

		//int nrToEmit = 1 + (m_current / (-m_psInfo->emission));

		//for (int i = 0; i < nrToEmit; i++)
		//{

		for (int i = 0; i < m_particle.size(); i++)
		{
			Particle& particles = m_particle.at(i);


			if (particles.time > 0.0f && particles.isAlive)
			{
				particles.time = particles.time - deltaTime;
				float pStatus = particles.time / m_psInfo.lifetime;
				float dragForce = pStatus / m_psInfo.drag;

				if (m_psInfo.omnious)
				{
					m_directionVector.at(i) = glm::normalize(particles.direction);
					particles.velocity = deltaTime * (m_psInfo.direction + (particles.rotation * m_psInfo.spread));

					//float offsetX;
					//float offsetY;
					//float offsetZ;
					////particles.position.x += static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 1 - 0.2;
					//offsetX = rand() % 2000 + 1 - 1000;
					//offsetX /= 1000;
					//offsetX *= 4;
					//particles.position.x += offsetX;
					////particles.position.y += static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 1 - 0.2;
					//offsetY = rand() % 2000 + 1 - 1000;
					//offsetY /= 2000;
					//particles.position.y += offsetY;
					////particles.position.z += static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 1 - 0.2;
					//offsetZ = rand() % 2000 + 1 - 1000;
					//offsetZ /= 1000;
					//offsetZ *= 4;
					//particles.position.z += offsetZ;
				}

				else
				{
					m_directionVector.at(i) = glm::normalize(particles.direction);
					particles.velocity = deltaTime * particles.rotation;
				}
				glm::vec3 otherPosition = particles.position;
				particles.position.y = particles.position.y + deltaTime * -m_psInfo.gravity;

				if (m_psInfo.drag != 0.0f)
				{
					particles.position.x = particles.position.x - (particles.velocity.x * dragForce * m_psInfo.force);
					particles.position.y = particles.position.y + (particles.velocity.y * dragForce * m_psInfo.force);
					particles.position.z = particles.position.z - (particles.velocity.z * dragForce * m_psInfo.force);
				}

				else
				{
					particles.position.x = particles.position.x - (particles.velocity.x * m_psInfo.force);
					particles.position.y = particles.position.y + (particles.velocity.y * m_psInfo.force);
					particles.position.z = particles.position.z - (particles.velocity.z * m_psInfo.force);
				}


				//---

				particles.direction = otherPosition - particles.position;

				//if(m_camera != nullptr)
				//	particles.distance = glm::length(particles.position - m_camera->getCamPos());
					//particles.distance = glm::length(particles.position - m_camera->getCamPos());

				m_vertex.at(i) = particles.position;
				m_lifetime.at(i) = pStatus;
			}
			else if (particles.isAlive && particles.time <= 0.0f)
			{
				particles.startLoop = false;
				particles.isAlive = false;
				if (m_psInfo.cont)
				{
					particles.time = m_psInfo.lifetime;
					m_lifetime.at(i) = particles.time / m_psInfo.lifetime;
				}
				//Reset variables
				particles.distance = -1.0f;
				particles.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
				particles.position = m_position;

				//---

									//float offsetX;
					//float offsetY;
					//float offsetZ;
					////particles.position.x += static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 1 - 0.2;
					//offsetX = rand() % 2000 + 1 - 1000;
					//offsetX /= 1000;
					//offsetX *= 4;
					//particles.position.x += offsetX;
					////particles.position.y += static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 1 - 0.2;
					//offsetY = rand() % 2000 + 1 - 1000;
					//offsetY /= 2000;
					//particles.position.y += offsetY;
					////particles.position.z += static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 1 - 0.2;
					//offsetZ = rand() % 2000 + 1 - 1000;
					//offsetZ /= 1000;
					//offsetZ *= 4;
					//particles.position.z += offsetZ;

				particles.direction = m_psInfo.direction;
				m_vertex.at(i) = particles.position;
				m_directionVector.at(i) = particles.direction;
			}
			else if (particles.isAlive == false && m_current <= 0.0f)
			{


				if (m_psInfo.cont == true)
				{
					m_nrOfActive += 1;
					particles.time = m_psInfo.lifetime;
					m_lifetime.at(i) = particles.time / m_psInfo.lifetime;
					particles.position = m_position;

					if (m_psInfo.randomSpawn == true)
					{
						float offsetX;
						float offsetY;
						float offsetZ;
						//particles.position.x += static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 1 - 0.2;
						offsetX = rand() % 2000 + 1 - 1000;
						offsetX /= 1000;
						offsetX *= 4;
						particles.position.x += offsetX;
						//particles.position.y += static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 1 - 0.2;
						offsetY = rand() % 2000 + 1 - 1000;
						offsetY /= 2000;
						particles.position.y += offsetY;
						//particles.position.z += static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 1 - 0.2;
						offsetZ = rand() % 2000 + 1 - 1000;
						offsetZ /= 1000;
						offsetZ *= 4;
						particles.position.z += offsetZ;
					}


				
					particles.distance = -1.0f;
					particles.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
					particles.isAlive = true;


					//---

					m_current += m_psInfo.emission * 2;
					m_vertex.at(i) = particles.position;
				}

				else
				{
					if (particles.startLoop)
					{
						m_nrOfActive += 1;
						particles.time = m_psInfo.lifetime;
						m_lifetime.at(i) = particles.time / m_psInfo.lifetime;
						particles.position = m_position;


						//---

						particles.distance = -1.0f;
						particles.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
						particles.isAlive = true;
						m_current += m_psInfo.emission;
						m_vertex.at(i) = particles.position;
					}
				}

			}
			if (particles.isAlive == false)
			{
				particles.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
				particles.position = glm::vec3(0.0f, -1000.0f, -20.0f);

				if (m_psInfo.cont == true)
					m_nrOfActive -= 1;
				m_vertex.at(i) = particles.position;
				m_lifetime.at(i) = 0.0f;
			}
			//particles.velocity *= 2;
		}
	}
}

void ParticleSystem::TempInit(PSinfo psInfo)
{
	m_psInfo = psInfo;
}

void ParticleSystem::Render(Camera* camera)
{
	ShaderMap::getInstance()->useByName(PARTICLES);

	m_camera = camera;
	//glUseProgram(ShaderMap::getInstance()->getShader(PARTICLES)->getShaderID());

	glm::mat4 VP = glm::mat4(1.0f);
	VP = camera->getProjMat() * camera->getViewMat();

	Shader* shader = ShaderMap::getInstance()->getShader(PARTICLES);

	shader->setMat4("WVP", VP); //Flipped order, check this!
	shader->setVec3("cam", camera->getCamPos());
	shader->setVec2("size", (glm::vec2(m_psInfo.width, m_psInfo.heigth)));
	shader->setInt("scaleDirection", m_psInfo.scaleDirection);
	shader->setInt("swirl", m_psInfo.swirl);
	shader->setFloat("glow", m_psInfo.glow);
	shader->setInt("fade", m_psInfo.fade);
	shader->setVec3("color", m_psInfo.color);
	shader->setVec3("blendColor", m_psInfo.blendColor);

	//What does this do in reality
	glUseProgram(m_shader);
	//bind vao
	glBindVertexArray(m_vao);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glUniform1i(glGetUniformLocation(m_shader, "ps_texture"), 0);
	glEnableVertexAttribArray(m_vertexPosition); //varje frame?
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertex.size() * sizeof(glm::vec3), &m_vertex[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(m_vertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(m_vertexDirection); //varje frame?
	glBindBuffer(GL_ARRAY_BUFFER, m_directionalBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_directionVector.size() * sizeof(glm::vec3), &m_directionVector[0], GL_STATIC_DRAW); //Is this correct?
	glVertexAttribPointer(m_vertexDirection, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const GLvoid*)0);
	glEnableVertexAttribArray(m_vertexLife); //varje frame?
	glBindBuffer(GL_ARRAY_BUFFER, m_lifetimeBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_lifetime.size() * sizeof(float), &m_lifetime[0], GL_STATIC_DRAW);
	glVertexAttribPointer(m_vertexLife, 1, GL_FLOAT, GL_FALSE, sizeof(float), (const GLvoid*)0);
	glDrawArrays(GL_POINTS, 0, m_vertex.size());
	glDisableVertexAttribArray(m_vertexPosition);
	glDisableVertexAttribArray(m_vertexDirection);
	glDisableVertexAttribArray(m_vertexLife);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glBindVertexArray(0);
	//unbind vao
}

void ParticleSystem::Pause()
{
	m_isPlaying = false;
}

void ParticleSystem::Play()
{
	m_isPlaying = true;
}

void ParticleSystem::SetPosition(glm::vec3 position)
{
	m_position = position;
}