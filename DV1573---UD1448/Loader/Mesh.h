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
	Mesh();
	~Mesh();
	BGLoader load;
	std::string filename;

public:
	void setUpMesh();


};