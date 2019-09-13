#pragma once
#include <Pch/Pch.h>
#include <Loader/MeshFormatInput.h>

class Mesh
{
private:
	int vertexCount;
	int faceCount;
	std::vector <Faces> faces;
	std::vector <Vertices> vertices;

	BGLoader load;
	std::string filename;
	
public:
	void setUpMesh(float*, int, int*, int);
	Mesh();
	~Mesh();

};

