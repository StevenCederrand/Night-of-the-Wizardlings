#include "Pch/Pch.h"
#include "HUDShader.h"

HUDShader::HUDShader(std::string vertex, std::string fragment)
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

	m_valid = true;
}

HUDShader::~HUDShader()
{
	glDeleteProgram(m_shaderProg);
}

void HUDShader::use()
{
	glUseProgram(m_shaderProg);

}

void HUDShader::unuse()
{
	glUseProgram(NULL);
}

int HUDShader::callTexture(std::string textureName)
{
	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(textureName.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return texture;
}

const int& HUDShader::Program() const
{
	return m_shaderProg;
	// TODO: insert return statement here
}

void HUDShader::setVec3(std::string name, glm::vec3 vec)
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

void HUDShader::setMat4(std::string name, glm::mat4 mat)
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

GLint HUDShader::getUniformLocation(std::string locationName)
{
	GLint location = -1;

	if (m_IDMap.find(locationName) != m_IDMap.end()) {
		location = m_IDMap[locationName];
	}

	return location;
}

int HUDShader::getShaderID() const
{
	return m_shaderProg;
}

void HUDShader::setInt(std::string name, int num)
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

void HUDShader::shaderSetup(std::string shaderName, unsigned int& shader)
{
	std::string Code;
	std::ifstream Shader;
	Shader.open(SHADERPATH + shaderName);

	if (!Shader.is_open())
	{
		logError("Failed to find shader file {0}", shaderName);
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
