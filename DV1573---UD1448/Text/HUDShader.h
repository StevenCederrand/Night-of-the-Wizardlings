
#pragma once
#include <Pch/Pch.h>
//#include <Texture/stb_image.h>


class HUDShader
{
public:
	HUDShader(std::string vertex, std::string fragment);
	~HUDShader();
	void use();
	void unuse();
	int callTexture(std::string textureName);
	const int& Program() const;
	void setVec3(std::string name, glm::vec3 vec);
	void setVec4(std::string name, glm::vec4 vec);
	void setMat4(std::string name, glm::mat4 mat);
	GLint getUniformLocation(std::string locationName);
	int getShaderID() const;
	void setInt(std::string name, int num);
	void setFloat(std::string name, float f);

private:

	int m_shaderProg;
	void shaderSetup(std::string shaderName, unsigned int& shader);
	bool m_valid;
	std::map<std::string, GLint> m_IDMap;

	std::vector<std::string> m_shaderNames; //We need to save the name of the shaders. 

};