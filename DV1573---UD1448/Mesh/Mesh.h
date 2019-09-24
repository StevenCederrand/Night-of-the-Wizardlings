#ifndef _MESH_h
#define _MESH_h
#include <Pch/Pch.h>

class Mesh
{
private:
	std::string m_name;
	std::string m_filePath;
	std::string m_materialName;
	int m_indexInFile;
	
	Transform m_transform;

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
	void setMaterial(std::string matName);

	// Returns mesh local position
	const Transform& getTransform() const { return m_transform; }


	void setPos(glm::vec3 pos);
	void setRot(glm::quat quat);
	void setScale(glm::vec3 scale);

	std::string getMaterial();

	Buffers getBuffers() const;

	int getVertexCount() const { return m_vertexCount; }
	int getFaceCount() const { return m_faceCount; }
	std::string getName() const { return m_name; }
	
	const std::vector<Face>& getFaces() { return m_faces; }
	const std::vector<Vertices>& getVertices() { return m_vertices; }


};

#endif