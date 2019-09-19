#ifndef _MESH_h
#define _MESH_h
#include <Pch/Pch.h>

class Mesh
{
private:
	std::string m_name;
	std::string m_filePath;
	int m_indexInFile;
	
	glm::vec3 m_worldPos;
	glm::quat m_worldRot;
	glm::vec3 m_worldScale;

	int m_vertexCount;
	int m_faceCount;
	std::vector<Face> m_faces;
	std::vector<Vertices> m_vertices;
	Buffers m_vertexBuffer;



public:
	Mesh();
	~Mesh();

	void setUpMesh(std::vector<Vertices> vertices, std::vector<Face> faces);
	void nameMesh(std::string name);
	void saveFilePath(std::string name, int index);
	void setUpBuffers();

	glm::vec3 getPos() const { return m_worldPos; }
	glm::quat getRot() const { return m_worldPos; }
	glm::vec3 getScale() const { return m_worldPos; }

	Buffers getBuffers() const;

	int getVertexCount() const { return m_vertexCount; }
	int getFaceCount() const { return m_faceCount; }
	
	const std::vector<Face>& getFaces() { return m_faces; }
	const std::vector<Vertices>& getVertices() { return m_vertices; }


};

#endif