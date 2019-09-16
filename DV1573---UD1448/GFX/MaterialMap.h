#ifndef _MATERIALMAP_h
#define _MATERIALMAP_h
#include <Pch/Pch.h>

class MaterialMap {

public:
	static MaterialMap* getInstance();
	void cleanUp();
	bool existsWithName(std::string name);
	void reload();
	void useByName(std::string name);
	Materials* getMaterial(std::string name);

	void destroy();

private:

	MaterialMap();
	static MaterialMap* m_materialMapInstance;
	std::map<std::string, Materials*> m_Materials;
};


#endif