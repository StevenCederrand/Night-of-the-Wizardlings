#include <Pch/Pch.h>
#include "Mesh.h"

Mesh::Mesh()
{
	name = "";

	m_worldPos = glm::vec3();
	m_worldRot = glm::quat();
	m_worldScale = glm::vec3();

	m_vertexCount = 0;
	m_faceCount = 0;
}

Mesh::~Mesh()
{

}

void Mesh::setUpMesh(std::vector<Vertices> vertices, std::vector<Face> faces)
{
	int j = 0;
	m_vertexCount = vertices.size();
	m_faceCount = faces.size();

	//vertices.resize(m_vertexCount);
	//faces.resize(m_inFaceCount);

	m_vertices = vertices;
	m_faces = faces;
}

void Mesh::setUpBuffers()
{
	glGenVertexArrays(1, &m_vertexBuffer.vao);
	glGenBuffers(1, &m_vertexBuffer.vbo);
	glGenBuffers(1, &m_vertexBuffer.ibo);

	glBindVertexArray(m_vertexBuffer.vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertices), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_faces.size() * sizeof(int) * 3,
		&m_faces[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)offsetof(Vertices, Normals));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)offsetof(Vertices, UV));
	glBindVertexArray(0);

	m_vertexBuffer.nrOfFaces = m_faces.size();
}


Buffers Mesh::getBuffers() const
{
	return m_vertexBuffer;
}
