#include <Pch/Pch.h>
#include "Shader.h"

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
		logError("ERROR WITH SHADER");
		logInfo(buffer);
		system("pause");
	}

	glDetachShader(m_shaderProg, vertexShader);
	glDetachShader(m_shaderProg, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

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
		logWarning("ERROR WITH SHADER");
		logInfo(buffer);
	}

	glDetachShader(m_shaderProg, vertexShader);
	glDetachShader(m_shaderProg, geometryShader);
	glDetachShader(m_shaderProg, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
	glDeleteProgram(m_shaderProg);
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
	GLint uniformLoc = glGetUniformLocation(this->getShaderID(), name.c_str());
	if (uniformLoc != -1)
	{
		glUniformMatrix3fv(uniformLoc, 1, GL_FALSE, &mat[0][0]);
	}
	else
	{
		logWarning("ERROR MAT3: ");
		logWarning(name);
	}
}
//uniform mat4
void Shader::setMat4(std::string name, glm::mat4 mat)
{
	GLint uniformLoc = glGetUniformLocation(this->getShaderID(), name.c_str());
	if (uniformLoc != -1)
	{
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, &mat[0][0]);
	}
	else
	{
		logWarning("ERROR MAT4: ");
		logWarning(name);
	}
}
//uniform vec3
void Shader::setVec3(std::string name, glm::vec3 vec)
{
	GLint uniformLoc = glGetUniformLocation(this->getShaderID(), name.c_str());
	if (uniformLoc != -1)
	{
		glUniform3fv(uniformLoc, 1, &vec[0]);
	}
	else
	{
		logWarning("ERROR VEC3: ");
		logWarning(name);
	}
}
//uniform vec4
void Shader::setVec4(std::string name, glm::vec4 vec)
{
	GLint uniformLoc = glGetUniformLocation(this->getShaderID(), name.c_str());
	if (uniformLoc != -1)
	{
		glUniform4fv(uniformLoc, 1, &vec[0]);
	}
	else
	{
		logWarning("ERROR VEC4: ");
		logWarning(name);
	}
}
//uniform float
void Shader::setFloat(std::string name, float num)
{
	GLint uniformLoc = glGetUniformLocation(this->getShaderID(), name.c_str());
	if (uniformLoc != -1)
	{
		glUniform1f(uniformLoc, num);
	}
	else
	{
		logWarning("ERROR FLOAT: ");
		logWarning(name);
	}
}
//uniform int
void Shader::setInt(std::string name, int num)
{
	GLint uniformLoc = glGetUniformLocation(this->getShaderID(), name.c_str());
	if (uniformLoc != -1)
	{
		glUniform1i(uniformLoc, num);
	}
	else
	{
		logWarning("ERROR INT: ");
		logWarning(name);
	}
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

//one function for all the shaders instead of one for each
void Shader::shaderSetup(std::string shaderName, unsigned int& shader)
{


	std::string Code;
	std::ifstream Shader;
	Shader.open(shaderName);

	if (!Shader.is_open())
	{
		logError("Failed to find shader file " + shaderName);
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

	if (compileResult == GL_FALSE)
	{
		glGetShaderInfoLog(shader, 1024, nullptr, buffer);
		logWarning("ERROR COMPILING: ");
		logWarning(shaderName);
		logInfo(buffer);
	}
}
