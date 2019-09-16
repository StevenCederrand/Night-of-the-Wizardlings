#include "Pch/Pch.h"
#include "Mesh.h"

Mesh::Mesh()
{
	name = "";

	m_worldPos = glm::vec3();
	m_worldRot = glm::quat();
	m_worldScale = glm::vec3();

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
	faceCount = inFaceCount;

	vertices.resize(vertexCount);
	faces.resize(inFaceCount);


	// Fill verts
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

	// Fill faces
	int* tempFace = inIndices;
	j = 0;
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

void Mesh::setUpBuffers()
{
	glGenVertexArrays(1, &vertexBuffer.vao);
	glGenBuffers(1, &vertexBuffer.vbo);
	glGenBuffers(1, &vertexBuffer.ibo);

	glBindVertexArray(vertexBuffer.vao);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertices), &vertices[0], GL_STATIC_DRAW); 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffer.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(int) * 3,
		&faces[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)offsetof(Vertices, Normals));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)offsetof(Vertices, UV));
	glBindVertexArray(0);

	vertexBuffer.nrOfFaces = faces.size();
}

Buffers Mesh::getBuffers() const
{
	return vertexBuffer;
}
