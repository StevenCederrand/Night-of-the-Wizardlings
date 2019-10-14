#include "Pch/Pch.h"
#include "DebugDrawer.h"

DebugDrawer::DebugDrawer()
{	
	m_mesh = nullptr;
}

DebugDrawer::~DebugDrawer()
{
	delete m_mesh;
}

void DebugDrawer::setUpMesh(btRigidBody& body)
{
	m_mesh = new Mesh();

	btVector3 tempMin;
	btVector3 tempMax;

	body.getAabb(tempMin, tempMax);

	std::vector<Vertex> vertices;
	std::vector<Face> faces;

	vertices.resize(8);
	faces.resize(12);

	vertices[0].position = glm::vec3(tempMin.getX(), tempMin.getY(), tempMin.getZ());
	vertices[1].position = glm::vec3(vertices[0].position.x + 100 * 2, vertices[0].position.y, vertices[0].position.z);
	vertices[2].position = glm::vec3(vertices[1].position.x, vertices[1].position.y, vertices[1].position.z + 100 * 2);
	vertices[3].position = glm::vec3(vertices[0].position.x, vertices[0].position.y, vertices[0].position.z + 100 * 2);
	vertices[4].position = glm::vec3(tempMax.getX(), tempMax.getY(), tempMax.getZ());
	vertices[5].position = glm::vec3(vertices[4].position.x - 100 * 2, vertices[4].position.y, vertices[4].position.z);
	vertices[6].position = glm::vec3(vertices[5].position.x, vertices[5].position.y, vertices[5].position.z - 100 * 2);
	vertices[7].position = glm::vec3(vertices[4].position.x, vertices[4].position.y, vertices[4].position.z - 100 * 2);

	faces[0].indices[0] = 0;
	faces[0].indices[1] = 1;
	faces[0].indices[2] = 3;

	faces[1].indices[0] = 3;
	faces[1].indices[2] = 1;
	faces[1].indices[0] = 2;

	faces[2].indices[0] = 1;
	faces[2].indices[1] = 2;
	faces[2].indices[2] = 7;

	faces[3].indices[0] = 7;
	faces[3].indices[1] = 2;
	faces[3].indices[2] = 4;

	faces[4].indices[0] = 2;
	faces[4].indices[1] = 3;
	faces[4].indices[2] = 4;

	faces[5].indices[0] = 4;
	faces[5].indices[1] = 3;
	faces[5].indices[2] = 5;

	faces[6].indices[0] = 5;
	faces[6].indices[1] = 3;
	faces[6].indices[2] = 6;

	faces[7].indices[0] = 6;
	faces[7].indices[1] = 3;
	faces[7].indices[2] = 0;

	faces[8].indices[0] = 0;
	faces[8].indices[1] = 1;
	faces[8].indices[2] = 7;

	faces[9].indices[0] = 7;
	faces[9].indices[1] = 0;
	faces[9].indices[2] = 6;

	faces[10].indices[0] = 6;
	faces[10].indices[1] = 5;
	faces[10].indices[2] = 7;

	faces[11].indices[0] = 7;
	faces[11].indices[1] = 5;
	faces[11].indices[2] = 4;

	m_mesh->setUpMesh(vertices, faces);
	m_mesh->setUpBuffers();
}

Buffers DebugDrawer::getBuffers()
{
	return m_mesh->getBuffers();
}