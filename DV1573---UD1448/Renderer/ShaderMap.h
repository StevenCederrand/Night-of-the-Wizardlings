#ifndef _SHADERMAP_H
#define _SHADERMAP_H
#include <Pch/Pch.h>

class ShaderMap {
public:
	static ShaderMap* getInstance();
	//@ vsName & fsName enter just the name of the shader.
	static Shader* createShader(std::string name, std::string vsName, std::string fsName);
	static Shader* getShader(std::string name);
	static void cleanUp();
	static bool existsWithName(std::string name);
	static void reload();
	static void useByName(std::string name);

	static void destroy();
private:
	ShaderMap();
	static ShaderMap* m_shaderMapInstance;
	static std::map<std::string, Shader*> m_shaderMap;
};




#endif