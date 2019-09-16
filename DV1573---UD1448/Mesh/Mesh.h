#pragma once
#include <Pch/Pch.h>

class Mesh
{
private:
	std::string name;
	
	glm::vec3 m_worldPos;
	glm::quat m_worldRot;
	glm::vec3 m_worldScale;

	int vertexCount;
	int faceCount;
	std::vector<Face> faces;
	std::vector<Vertices> vertices;
	Buffers vertexBuffer;

	int materialCount;
	std::vector<Materials> materials;

	std::string materialName;


public:
	Mesh();
	~Mesh();

	void setUpMesh(float* inVertices, int inVertexCount, int* inIndices, int inFaceCount);
	void setUpBuffers();
	void setUpMaterials();

	glm::vec3 getPos() const { return m_worldPos; }
	glm::quat getRot() const { return m_worldPos; }
	glm::vec3 getScale() const { return m_worldPos; }

	Buffers getBuffers() const;

	int getVertexCount() const { return vertexCount; }
	int getFaceCount() const { return faceCount; }
	
	const std::vector<Face>& getFaces() { return faces; }
	const std::vector<Vertices>& getVertices() { return vertices; }


};

