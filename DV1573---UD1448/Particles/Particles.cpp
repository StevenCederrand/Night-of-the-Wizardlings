#include "Pch/Pch.h"
#include "Particles.h"

#include <random>

ParticleSystem::ParticleSystem(PSinfo* psInfo, TextureInfo* txtInfo, glm::vec3 position, GLuint shader,
	psBuffers psBuffer, std::vector<glm::vec3> vertex, std::vector<glm::vec3> directionVector, std::vector<Particle> particle, std::vector<float> lifetime)
{
	//TextureInfo rings;
	//rings.name = "Assets/Textures/Spell_1.png";
	//rings.width = 64;
	//rings.height = 64;

	m_txtInfo = txtInfo;//&rings;
	m_psInfo = psInfo;
	m_position = glm::vec3(0, -100, 0);
	m_shader = shader;
	m_current = psInfo->emission;

	//Instead of 0 this now gets data from the buffer class
	m_texture = psBuffer.texture;
	m_vertexBuffer = psBuffer.vertexBuffer;
	m_directionalBuffer = psBuffer.directionalBuffer;
	m_lifetimeBuffer = psBuffer.lifetimeBuffer;
	m_vertexPosition = psBuffer.vertexPosition;
	m_vertexDirection = psBuffer.vertexDirection;
	m_vertexLife = psBuffer.vertexLife;

	//This is for the vector info
	m_vertex = vertex;
	m_directionVector = directionVector;
	m_particle = particle;
	m_lifetime = lifetime;

	m_vao = psBuffer.vao;

	Initialize();
}

//ParticleSystem::ParticleSystem()
//{
//	m_txtInfo = nullptr;
//	m_psInfo = nullptr;
//	m_position = glm::vec3(0, 0, 0);
//	m_shader = 0;
//	m_current = 0;
//
//	m_texture = 0;
//	m_vertexBuffer = 0;
//	m_directionalBuffer = 0;
//	m_lifetimeBuffer = 0;
//	m_vertexPosition = 0;
//	m_vertexDirection = 0;
//	m_vertexLife = 0;
//
//	m_cont = 0;
//	m_direction = glm::vec3(0, 0, 0);
//	m_isPlaying = true;
//	m_nrOfActive = 0;
//	m_quad = glm::mat4(0);
//	m_lookAt = glm::mat4(0);
//}

ParticleSystem::~ParticleSystem()
{
	m_psInfo = NULL;
	m_txtInfo = NULL;
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

PSinfo* ParticleSystem::GetParticleInfo()
{
	return m_psInfo;
}

TextureInfo* ParticleSystem::GetTextureInfo()
{
	return m_txtInfo;
}

glm::mat4 ParticleSystem::GetQuad()
{
	return m_quad;
}

bool ParticleSystem::Initialize()
{
	//random_device rd;
	////////std::mt19937 gen(m_psInfo->seed);
	////////std::uniform_real_distribution<double> dis(-1.0f, 1.0f);
	////////for (int i = 0; i < m_psInfo->maxParticles; i++)
	////////{
	////////	float x = dis(gen);
	////////	float y = dis(gen);
	////////	float z = dis(gen);

	////////	Particle particles;
	////////	glm::vec3 v = m_position;

	////////	particles.position = m_position;
	////////	particles.distance = -1.0f;
	////////	particles.isAlive = false;
	////////	particles.startLoop = true;
	////////	particles.rotation = glm::vec3(x, y, z);
	////////	particles.time = m_psInfo->lifetime;
	////////	particles.velocity = glm::vec3(0.0f, 0.0f, 0.0f);

	////////	//Pushbacks
	////////	m_vertex.push_back(v);
	////////	m_particle.push_back(particles);
	////////	m_directionVector.push_back(particles.rotation);
	////////	m_lifetime.push_back(particles.time);
	////////}

	m_isPlaying = true;

	//m_txtInfo->name = "Assets/Textures/Spell_1.png";
	//m_txtInfo->height = 64;
	//m_txtInfo->width = 64;


	//This is done in the ParticleBuffers class instead!
	//////////if (m_txtInfo != NULL)
	//////////{
	//////////	glGenTextures(1, &m_texture);
	//////////	glBindTexture(GL_TEXTURE_2D, m_texture);
	//////////	unsigned char* imageData = stbi_load(m_txtInfo->name.c_str(), &m_txtInfo->width, &m_txtInfo->height, &m_txtInfo->nrChannels, 0); //String to const char, might need a check if problemo
	//////////	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_txtInfo->width, m_txtInfo->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	//////////	stbi_image_free(imageData);
	//////////	glUniform1i(glGetUniformLocation(m_shader, "ps_texture"), 0);
	//////////}

	//////////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//////////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//////////m_quad = glm::mat4(1.0f);

	////////////gen vao
	//////////glGenVertexArrays(1, &m_vao);
	//////////glBindVertexArray(m_vao);
	////////////bind vao
	//////////glGenBuffers(1, &m_vertexBuffer);
	//////////glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	//////////glBufferData(GL_ARRAY_BUFFER, m_vertex.size() * sizeof(glm::vec3), &m_vertex[0], GL_STATIC_DRAW);
	//////////m_vertexPosition = glGetAttribLocation(m_shader, "vtxPos");
	//////////glGenBuffers(1, &m_directionalBuffer);
	//////////glBindBuffer(GL_ARRAY_BUFFER, m_directionalBuffer);
	//////////glBufferData(GL_ARRAY_BUFFER, m_directionVector.size() * sizeof(glm::vec3), &m_direction[0], GL_STATIC_DRAW);
	//////////m_vertexDirection = glGetAttribLocation(m_shader, "vtxDir");
	//////////glGenBuffers(1, &m_lifetimeBuffer);
	//////////glBindBuffer(GL_ARRAY_BUFFER, m_lifetimeBuffer);
	//////////glBufferData(GL_ARRAY_BUFFER, m_lifetime.size() * sizeof(float), &m_lifetime[0], GL_STATIC_DRAW);
	//////////m_vertexLife = glGetAttribLocation(m_shader, "vtxLifetime");
	////////////unbind vao
	//////////glBindVertexArray(0);

	//TODO
	//Here I can implement the code for randomizing the position in the flamestrike's circle

	auto rnd = rand() % 999 + 1; // random distance
	rnd /= 1000;
	auto offset = rnd * 10 * glm::sqrt(rnd); //circleRadius = 2?

	rnd = rand() % 999 + 1;
	rnd /= 1000;
	auto angle = 2.0f * glm::pi<float>() * rnd; // Random angle

	m_position.x += offset * glm::cos(angle);
	m_position.y += offset * glm::sin(angle);

	return true;
}

bool ParticleSystem::Build(PSinfo* psInfo)
{
	m_psInfo = psInfo;
	m_vertex.resize(m_psInfo->maxParticles);
	m_particle.resize(m_psInfo->maxParticles);
	m_directionVector.resize(m_psInfo->maxParticles);
	m_lifetime.resize(m_psInfo->maxParticles);
	return true;
}

bool ParticleSystem::Texture(TextureInfo* txtInfo)
{
	m_texture = 0;
	m_txtInfo = txtInfo;
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	unsigned char* imageData = stbi_load(m_txtInfo->name.c_str(), &m_txtInfo->width, &m_txtInfo->height, &m_txtInfo->nrChannels, 0); //String to const char, might need a check if problemo
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_txtInfo->width, m_txtInfo->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
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

void ParticleSystem::Update(PSinfo* psInfo, glm::vec3 cameraPos, float time)
{
	if (m_isPlaying == true)
	{
		float deltaTime = time;
		m_nrOfActive = m_psInfo->maxParticles;
		m_psInfo = psInfo;

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
				float pStatus = particles.time / m_psInfo->lifetime;
				float dragForce = pStatus / m_psInfo->drag;

				if (m_psInfo->omnious)
				{
					m_directionVector.at(i) = glm::normalize(particles.direction);
					particles.velocity = deltaTime * (m_psInfo->direction + (particles.rotation * m_psInfo->spread));
				}

				else
				{
					m_directionVector.at(i) = glm::normalize(particles.direction);
					particles.velocity = deltaTime * particles.rotation;
				}
				glm::vec3 otherPosition = particles.position;
				particles.position.y = particles.position.y + deltaTime * -m_psInfo->gravity;

				if (m_psInfo->drag != 0.0f)
				{
					particles.position.x = particles.position.x - (particles.velocity.x * dragForce * m_psInfo->force);
					particles.position.y = particles.position.y + (particles.velocity.y * dragForce * m_psInfo->force);
					particles.position.z = particles.position.z - (particles.velocity.z * dragForce * m_psInfo->force);
				}

				else
				{
					particles.position.x = particles.position.x - (particles.velocity.x * m_psInfo->force);
					particles.position.y = particles.position.y + (particles.velocity.y * m_psInfo->force);
					particles.position.z = particles.position.z - (particles.velocity.z * m_psInfo->force);
				}


				//---

				particles.direction = otherPosition - particles.position;
				particles.distance = glm::length(particles.position - cameraPos);
				m_vertex.at(i) = particles.position;
				m_lifetime.at(i) = pStatus;
			}
			else if (particles.isAlive && particles.time <= 0.0f)
			{
				particles.startLoop = false;
				particles.isAlive = false;
				if (m_psInfo->cont)
				{
					particles.time = m_psInfo->lifetime;
					m_lifetime.at(i) = particles.time / m_psInfo->lifetime;
				}
				//Reset variables
				particles.distance = -1.0f;
				particles.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
				particles.position = m_position;

				//---

				particles.direction = m_psInfo->direction;
				m_vertex.at(i) = particles.position;
				m_directionVector.at(i) = particles.direction;
			}
			else if (particles.isAlive == false && m_current <= 0.0f)
			{


				if (m_psInfo->cont == true)
				{
					m_nrOfActive += 1;
					particles.time = m_psInfo->lifetime;
					m_lifetime.at(i) = particles.time / m_psInfo->lifetime;
					particles.position = m_position;

					float offsetX;
					float offsetY;
					float offsetZ;
					//particles.position.x += static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 1 - 0.2;
					offsetX = rand() % 1999 + 1 -1000;
					offsetX /= 1000;
					offsetX *= 4;
					particles.position.x += offsetX;
					//particles.position.y += static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 1 - 0.2;
					offsetY = rand() % 1999 + 1 - 1000;
					offsetY /= 2000;
					particles.position.y += offsetY;
					//particles.position.z += static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 1 - 0.2;
					offsetZ = rand() % 1999 + 1 - 1000;
					offsetZ /= 1000;
					offsetZ *= 4;
					particles.position.z += offsetZ;

				
					particles.distance = -1.0f;
					particles.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
					particles.isAlive = true;


					//---

					m_current += m_psInfo->emission * 2;
					m_vertex.at(i) = particles.position;
				}

				else
				{
					if (particles.startLoop)
					{
						m_nrOfActive += 1;
						particles.time = m_psInfo->lifetime;
						m_lifetime.at(i) = particles.time / m_psInfo->lifetime;
						particles.position = m_position;


						//---

						particles.distance = -1.0f;
						particles.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
						particles.isAlive = true;
						m_current += m_psInfo->emission;
						m_vertex.at(i) = particles.position;
					}
				}

			}
			if (particles.isAlive == false)
			{
				particles.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
				particles.position = glm::vec3(0.0f, -1000.0f, 0.0f);

				if (m_psInfo->cont == true)
					m_nrOfActive -= 1;
				m_vertex.at(i) = particles.position;
				m_lifetime.at(i) = 0.0f;
			}
			//particles.velocity *= 2;
		}
	}
}

void ParticleSystem::TempInit(PSinfo* psInfo)
{
}

void ParticleSystem::Render(const Camera* camera, const PSinfo* psInfo)
{
	ShaderMap::getInstance()->useByName(PARTICLES);
	//glUseProgram(ShaderMap::getInstance()->getShader(PARTICLES)->getShaderID());
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	//bindPS(PARTICLES);


	//glUseProgram(ShaderMap::getInstance()->getShader(PARTICLES)->getShaderID());

	glm::mat4 VP = glm::mat4(1.0f);
	//VP = m_camera->getViewMat() * m_camera->getProjMat();
	VP = camera->getProjMat() * camera->getViewMat();


	ShaderMap::getInstance()->getShader(PARTICLES)->setMat4("WVP", VP); //Flipped order, check this!
	ShaderMap::getInstance()->getShader(PARTICLES)->setVec3("cam", camera->getCamPos());
	ShaderMap::getInstance()->getShader(PARTICLES)->setVec2("size", (glm::vec2(psInfo->width, psInfo->heigth)));
	ShaderMap::getInstance()->getShader(PARTICLES)->setInt("scaleDirection", psInfo->scaleDirection);
	ShaderMap::getInstance()->getShader(PARTICLES)->setInt("swirl", psInfo->swirl);
	ShaderMap::getInstance()->getShader(PARTICLES)->setInt("glow", psInfo->glow);
	ShaderMap::getInstance()->getShader(PARTICLES)->setInt("fade", psInfo->fade);
	ShaderMap::getInstance()->getShader(PARTICLES)->setVec3("color", psInfo->color);
	ShaderMap::getInstance()->getShader(PARTICLES)->setVec3("blendColor", psInfo->blendColor);




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

//void ParticleSystem::LoadInfo(unsigned int texture, unsigned int vao, unsigned int vertexBuffer, unsigned int directionalBuffer, 
//	unsigned int lifetimeBuffer, unsigned int vertexPos, unsigned int vertexDir, unsigned int vertexLife, 
//	std::vector<glm::vec3> vertex, std::vector<glm::vec3> directionVector, std::vector<Particle> particle, std::vector<float> lifetime)
//{
//	m_texture = texture;
//	m_vertexBuffer = vertexBuffer;
//	m_directionalBuffer = directionalBuffer;
//	m_lifetimeBuffer = lifetimeBuffer;
//	m_vertexPosition = vertexPos;
//	m_vertexDirection = vertexDir;
//	m_vertexLife = vertexLife;
//
//	//This is for the vector info
//	m_vertex = vertex;
//	m_directionVector = directionVector;
//	m_particle = particle;
//	m_lifetime = lifetime;
//
//	m_vao = vao;
//}

void ParticleSystem::LoadInfo(psBuffers psBuffer)
{
	m_texture = psBuffer.texture;
	m_vertexBuffer = psBuffer.vertexBuffer;
	m_directionalBuffer = psBuffer.directionalBuffer;
	m_lifetimeBuffer = psBuffer.lifetimeBuffer;
	m_vertexPosition = psBuffer.vertexPosition;
	m_vertexDirection = psBuffer.vertexDirection;
	m_vertexLife = psBuffer.vertexLife;
	m_vao = psBuffer.vao;
}

void ParticleSystem::SetPosition(glm::vec3 position)
{
	m_position = position;
}