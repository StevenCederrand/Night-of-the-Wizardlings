#include <Pch/Pch.h>
#include "Mesh.h"

Mesh::Mesh()
{
	m_name = "";
	m_filePath = "";
	m_materialName = "";
	m_skeleton = "";
	m_indexInFile = 0;

	m_vertexCount = 0;
	m_faceCount = 0;
}

Mesh::~Mesh()
{

}

void Mesh::setUpMesh(std::vector<Vertex> vertices, std::vector<Face> faces)
{
	int j = 0;
	m_vertexCount = (int)vertices.size();
	m_faceCount = (int)faces.size();

	m_vertices = vertices;
	m_faces = faces;
}

void Mesh::nameMesh(std::string name)
{
	m_name = name;
}

void Mesh::saveFilePath(std::string name, int index)
{
	m_filePath = name;
	m_indexInFile = index;
}

void Mesh::setUpBuffers()
{
	glGenVertexArrays(1, &m_vertexBuffer.vao);
	glGenBuffers(1, &m_vertexBuffer.vbo);
	glGenBuffers(1, &m_vertexBuffer.ibo);

	glBindVertexArray(m_vertexBuffer.vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_faces.size() * sizeof(int) * 3,
		&m_faces[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normals));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));
	glBindVertexArray(0);

	m_vertexBuffer.nrOfFaces = static_cast<int>(m_faces.size());

}

void Mesh::setMaterial(std::string matName)
{
	m_materialName = matName;
}

void Mesh::addAnimation(std::string name)
{
	m_animations.push_back(name);
}

void Mesh::setSkeleton(std::string name)
{
	m_skeleton = name;
}

void Mesh::setPos(glm::vec3 pos)
{
	m_transform.position = pos;
}

void Mesh::setRot(glm::quat quat)
{
	m_transform.rotation = quat;
}

void Mesh::setScale(glm::vec3 scale)
{
	m_transform.scale = scale;
}

void Mesh::setTransform(Transform transform)
{
	m_transform = transform;
}

std::string Mesh::getMaterial()
{
	return m_materialName;
}

Buffers Mesh::getBuffers() const
{
	return m_vertexBuffer;
}
