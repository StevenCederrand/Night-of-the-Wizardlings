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
	int j = 0;
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

	int* tempFace = inIndices;
	faceCount = inFaceCount;
	faces.resize(faceCount);


	int k = 0;

	for (int i = 0; i < faceCount; i++)
	{

		faces[i].indices[0] = tempFace[k]; k++;
		faces[i].indices[1] = tempFace[k]; k++;
		faces[i].indices[2] = tempFace[k]; k++;
	}




}
