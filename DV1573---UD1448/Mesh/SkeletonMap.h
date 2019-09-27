#ifndef _SKELETONMAP_H
#define _SKELETONMAP_H
#include <Pch/Pch.h>
#include <Mesh/MeshFormat.h>

class SkeletonMap
{
public:
	static SkeletonMap* getInstance();
	void cleanUp();
	bool existsWithName(std::string name);
	


};

