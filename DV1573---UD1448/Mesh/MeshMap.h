#ifndef _MATERIALMAP_h
#define _MATERIALMAP_h
#include <Pch/Pch.h>
#include <Mesh/MeshFormat.h>
#include <Mesh/Mesh.h>

class MeshMap {

public:
	static MeshMap* getInstance();
	void cleanUp();
	bool existsWithName(std::string name);
	void useByName(std::string name);
	Mesh* getMaterial(std::string name);
	Mesh* createMaterial(std::string name, Mesh mesh);
	void destroy();

private:

	MaterialMap();
	static MaterialMap* m_materialMapInstance;
	std::map<std::string, Mesh*> m_Mesh;
};


#endif