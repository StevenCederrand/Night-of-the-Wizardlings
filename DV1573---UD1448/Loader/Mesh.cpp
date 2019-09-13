#include "Pch/Pch.h"



Mesh::Mesh()
{
	vertexCount = 0;
	faceCount = 0;
	
}

Mesh::~Mesh()
{
}

void Mesh::setUpMesh(float* inVertices, int inVertexCount, int* inIndices, int inFaceCount)
{
	int j = 1;
	vertexCount = inVertexCount;
	vertices.resize(vertexCount);
	float* tempVert = inVertices;
	for (int i = 0; i < vertexCount; i++)
	{

		vertices[i].position[0] = tempVert[j]; j++;
		vertices[i].position[1] = tempVert[j]; j++;
		vertices[i].position[2] = tempVert[j]; j++;
			
		vertices[i].UV[0] = tempVert[j]; j++;
		vertices[i].UV[1] = tempVert[j]; j++;

		vertices[i].Normals[0] = tempVert[j]; j++;
		vertices[i].Normals[1] = tempVert[j]; j++;
		vertices[i].Normals[2] = tempVert[j]; j++;
	}
	j = 0;
	int* tempFace = inIndices;
	faces.resize(inFaceCount);


	int k = 0;

	for (int i = 0; i < inFaceCount; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			faces[i].indices[j] = tempFace[k];
			k++;
		}
	}




}
