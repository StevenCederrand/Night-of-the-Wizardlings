#include <Pch/Pch.h>
#include "Shader.h"

Shader::Shader() {

}

Shader::Shader(std::string compute)
{
	unsigned int computeShader = glCreateShader(GL_COMPUTE_SHADER);
	
	GLint compileResult;
	char buffer[1024];
	shaderSetup(compute, computeShader);
	m_shaderProg = glCreateProgram();
	glAttachShader(m_shaderProg, computeShader);
	glLinkProgram(m_shaderProg);
	
	glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE)
	{
		memset(buffer, 0, 1024);
		glGetProgramInfoLog(m_shaderProg, 1024, nullptr, buffer);
		logError("Error compiling shaders (computeShader)", compute);
		logInfo(buffer);
		m_valid = false;
	}
	glDetachShader(m_shaderProg, computeShader);
	glDeleteShader(computeShader);
}

Shader::Shader(std::string vertex, std::string fragment)
{

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	GLint compileResult;
	char buffer[1024];
	shaderSetup(vertex, vertexShader);
	shaderSetup(fragment, fragmentShader);

	m_shaderProg = glCreateProgram();
	glAttachShader(m_shaderProg, vertexShader);
	glAttachShader(m_shaderProg, fragmentShader);
	glLinkProgram(m_shaderProg);

	glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE)
	{
		memset(buffer, 0, 1024);
		glGetProgramInfoLog(m_shaderProg, 1024, nullptr, buffer);
		logError("Error compiling shaders (vertexShader) {0} and (fragmentShader) {1} ", vertex, fragment);
		logInfo(buffer);
		m_valid = false;
	}
	
	glDetachShader(m_shaderProg, vertexShader);
	glDetachShader(m_shaderProg, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	m_name = vertex + " " + fragment;
	m_valid = true;

}

Shader::Shader(std::string vertex, std::string geometry, std::string fragment)
{

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLint compileResult;
	char buffer[1024];
	//m_shaderSetUp does Vertex, geometry and fragment shader setup
	shaderSetup(vertex, vertexShader);
	shaderSetup(geometry, geometryShader);
	shaderSetup(fragment, fragmentShader);

	m_shaderProg = glCreateProgram();
	glAttachShader(m_shaderProg, vertexShader);
	glAttachShader(m_shaderProg, geometryShader);
	glAttachShader(m_shaderProg, fragmentShader);
	glLinkProgram(m_shaderProg);

	glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE)
	{
		memset(buffer, 0, 1024);
		glGetProgramInfoLog(m_shaderProg, 1024, nullptr, buffer);
		logError("Error compiling shaders (vertexShader) {0}, (geometryShader) {1} and (fragmentShader) {2} ", vertex, geometry, fragment);
		logInfo(buffer);
		m_valid = false;
	}

	glDetachShader(m_shaderProg, vertexShader);
	glDetachShader(m_shaderProg, geometryShader);
	glDetachShader(m_shaderProg, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);
	m_valid = true;

}


Shader::~Shader()
{
	glDeleteProgram(m_shaderProg);
	clearIDs();
}

void Shader::use()
{
	glUseProgram(m_shaderProg);
}

void Shader::unuse()
{
	glUseProgram(NULL);
}

//uniform mat3
void Shader::setMat3(std::string name, glm::mat3 mat)
{
	GLint uniformLoc = getUniformLocation(name);
	if (uniformLoc == -1)
	{
		uniformLoc = glGetUniformLocation(this->getShaderID(), name.c_str());

		if (uniformLoc == -1)
		{
			logError("Could not find uniform {0}", name);
			return;
		}

		m_IDMap[name] = uniformLoc; //Save the ID to the hashmap
	}

	glUniformMatrix3fv(uniformLoc, 1, GL_FALSE, &mat[0][0]);
}
//uniform mat4
void Shader::setMat4(std::string name, glm::mat4 mat)
{
	GLint uniformLoc = getUniformLocation(name);
	if (uniformLoc == -1)
	{
		uniformLoc = glGetUniformLocation(this->getShaderID(), name.c_str());

		if (uniformLoc == -1)
		{
			logError("Could not find uniform {0}", name);
			return;
		}

		m_IDMap[name] = uniformLoc; //Save the ID to the hashmap
	}

	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, &mat[0][0]);
}
void Shader::setVec2(std::string name, glm::vec2 vec)
{
	GLint uniformLoc = getUniformLocation(name);
	if (uniformLoc == -1)
	{
		uniformLoc = glGetUniformLocation(this->getShaderID(), name.c_str());

		if (uniformLoc == -1)
		{
			logError("Could not find uniform {0} in shader {1}", name, m_name);

			return;
		}

		m_IDMap[name] = uniformLoc; //Save the ID to the hashmap
	}

	glUniform2fv(uniformLoc, 1, &vec[0]);
}
//uniform vec3
void Shader::setVec3(std::string name, glm::vec3 vec)
{
	GLint uniformLoc = getUniformLocation(name);
	if (uniformLoc == -1)
	{
		uniformLoc = glGetUniformLocation(this->getShaderID(), name.c_str());

		if (uniformLoc == -1)
		{
			logError("Could not find uniform {0}", name);
			return;
		}

		m_IDMap[name] = uniformLoc; //Save the ID to the hashmap
	}

	glUniform3fv(uniformLoc, 1, &vec[0]);
}
//uniform vec4
void Shader::setVec4(std::string name, glm::vec4 vec)
{
	GLint uniformLoc = getUniformLocation(name);
	if (uniformLoc == -1)
	{
		uniformLoc = glGetUniformLocation(this->getShaderID(), name.c_str());

		if (uniformLoc == -1)
		{
			logError("Could not find uniform {0}", name);
			return;
		}

		m_IDMap[name] = uniformLoc; //Save the ID to the hashmap
	}

	glUniform3fv(uniformLoc, 1, &vec[0]);

}
//uniform float
void Shader::setFloat(std::string name, float num)
{
	GLint uniformLoc = getUniformLocation(name);
	if (uniformLoc == -1)
	{
		uniformLoc = glGetUniformLocation(this->getShaderID(), name.c_str());
		
		if (uniformLoc == -1) 
		{
			logError("Could not find uniform {0}", name);
			return;
		}
		
		m_IDMap[name] = uniformLoc; //Save the ID to the hashmap
	}

	glUniform1f(uniformLoc, num);

}
//uniform int
void Shader::setInt(std::string name, int num)
{
	GLint uniformLoc = getUniformLocation(name);
	if (uniformLoc == -1)
	{
		uniformLoc = glGetUniformLocation(this->getShaderID(), name.c_str());

		if (uniformLoc == -1)
		{
			logError("Could not find uniform {0}", name);
			return;
		}

		m_IDMap[name] = uniformLoc; //Save the ID to the hashmap
	}

	glUniform1i(uniformLoc, num);
}

//Assumption is that you are using the shader
void Shader::setMaterial(const std::string& materialName) {

	//If material names are the same
	if (m_oldMaterial == materialName) {
		return;
	}
	m_oldMaterial = materialName;

	Material* mat = MaterialMap::getInstance()->getMaterial(materialName);
	setVec3("Ambient_Color", mat->ambient);
	setVec3("Diffuse_Color", mat->diffuse);
	//setVec3("Specular_Color", mat->specular);

	setVec2("TexAndRim", glm::vec2(1, 1));
	
	for (size_t i = 0; i < mat->textureID.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, mat->textureID.at(i));
	}
	
}

void Shader::setMaterial(Material* material)
{
	setVec3("Ambient_Color", material->ambient);
	setVec3("Diffuse_Color", material->diffuse);
	setVec2("TexAndRim", glm::vec2(material->texture, 1));
	//setVec3("Specular_Color", mat->specular);

	for (size_t i = 0; i < material->textureID.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, material->textureID.at(i));
	}
}


void Shader::clearIDs() {
	m_IDMap.clear();
}

bool Shader::getValid() const
{
	return m_valid;
}

int Shader::getShaderID() const
{
	return m_shaderProg;
}

void Shader::setName(std::string name)
{
	m_name = name;
}

std::string Shader::getName() const
{
	return m_name;
}

std::vector<std::string> Shader::getShaderNames() const
{
	return m_shaderNames;
}

GLint Shader::getUniformLocation(std::string locationName)
{
	GLint location = -1;

	if (m_IDMap.find(locationName) != m_IDMap.end()) {
		location = m_IDMap[locationName];
	}

	return location;
}

//one function for all the shaders instead of one for each
void Shader::shaderSetup(std::string shaderName, unsigned int& shader)
{
	std::string Code;
	std::ifstream Shader;
	Shader.open(SHADERPATH + shaderName);

	if (!Shader.is_open())
	{
		logError("Failed to find shader file {0}" , shaderName);
	}

	std::stringstream Stream;
	Stream << Shader.rdbuf();
	Shader.close();

	Code = Stream.str();

	GLint compileResult;
	char buffer[1024];

	const char* shaderPtr = Code.c_str();
	glShaderSource(shader, 1, &shaderPtr, nullptr);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);

	m_shaderNames.push_back(shaderName);

	if (compileResult == GL_FALSE)
	{
		glGetShaderInfoLog(shader, 1024, nullptr, buffer);
		logWarning("ERROR COMPILING: ");
		logWarning(shaderName);
		logInfo(buffer);
	}
}

Shader& Shader::operator=(const Shader& other) {

	if (&other == this) {
		return *this;
	}
	else {
		//Clear and remove the shader program
		glDeleteProgram(m_shaderProg);
		clearIDs();

		m_shaderProg = other.m_shaderProg;
		m_IDMap = other.m_IDMap;
		m_name = other.m_name;
		m_shaderNames = other.m_shaderNames;
		return *this;
	}
}