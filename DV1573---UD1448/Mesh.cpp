#include "Pch/Pch.h"


Mesh::Mesh()
{
	vertexCount = 0;
	faceCount = 0;
	
}

Mesh::~Mesh()
{
}

void Mesh::setUpMesh()
{
	int j = 0;
	vertexCount = load.GetVertexCount(0);
	vertices.resize(vertexCount);
	float* tempVert = load.GetVertices(0);
	for (int i = 0; i < vertexCount; i++)
	{
		for (j; j < 8; j+=8)
		{
			for (int k = 0; k < 3; k++)
			{
				vertices[i].position[k] = tempVert[j];
				vertices[i].position[k + 1] = tempVert[j + 1];
				vertices[i].position[k + 2] = tempVert[j + 2];
			}
			for (int k = 0; k < 2; k++)
			{
				vertices[i].UV[k] = tempVert[j + 3];
				vertices[i].UV[k + 1] = tempVert[j + 4];
			}
			for (int k = 0; k < 3; k++)
			{
				vertices[i].Normals[k] = tempVert[j + 5];
				vertices[i].Normals[k + 1] = tempVert[j + 6];
				vertices[i].Normals[k + 2] = tempVert[j + 7];
			}


		}

	}

	int* tempFace = load.GetFaces(0);
	faceCount = load.GetVertexCount(0);

	int k = 0;

	for (int i = 0; i < faceCount; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			faces[i].indices[j] = tempFace[k];
			k++;
		}
	}




}
