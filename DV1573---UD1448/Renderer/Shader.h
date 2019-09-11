#ifndef _SHADER_H
#define _SHADER_H
#include <Pch/Pch.h>

class Shader
{
public:
	Shader();
	Shader(std::string vertex, std::string fragment);
	Shader(std::string vertex, std::string geometry, std::string fragment);
	~Shader();

	void use();
	void unuse();

	void setMat3(std::string name, glm::mat3 mat);
	void setMat4(std::string name, glm::mat4 mat);
	void setVec3(std::string name, glm::vec3 vec);
	void setVec4(std::string name, glm::vec4 vec);
	void setFloat(std::string name, float num);
	void setInt(std::string name, int num);
	void clearIDs();
	int getShaderID() const;
	void setName(std::string name);
	void reload();
	std::string getName() const;

	GLint getUniformLocation(std::string locationName);

	Shader& operator=(const Shader& other);
	bool createShader(std::string vertex, std::string fragment);

private:

	void shaderSetup(std::string shaderName, unsigned int& shader);

	std::string m_name;
	int m_shaderProg;
	std::map<std::string, GLint> m_IDMap;
	std::vector<std::string> m_shaderNames; //We need to save the name of the shaders. 
};

#endif