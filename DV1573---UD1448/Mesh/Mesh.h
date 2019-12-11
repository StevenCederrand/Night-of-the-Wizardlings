#ifndef _MESH_h
#define _MESH_h
#include <Pch/Pch.h>
#include <Loader/BGLoader.h>

class Mesh
{
private:
	std::string m_name;
	std::string m_filePath;
	std::string m_materialName;
	std::string m_skeleton;						//Left blank if none
	std::vector<std::string> m_animations;		//Left empty if none

	int m_indexInFile;
	
	int m_vertexCount;
	int m_faceCount;
	std::vector<Face> m_faces;
	std::vector<Vertex> m_vertices;
	std::vector<Vertex2> m_skeleVertices;		// Animation specific vertices	
	Buffers m_vertexBuffer;

public:
	Mesh();
	~Mesh();
	void Destroy();

	void setUpMesh(std::vector<Vertex> vertices, std::vector<Face> faces);
	void setUpMesh(std::vector<Vertex2> vertices, std::vector<Face> faces);
	void loadMesh(std::string fileName);
	void nameMesh(std::string name);
	void saveFilePath(std::string name, int index);
	void setUpBuffers();
	void setUpSkeleBuffers();					// Animation specific buffers
	void setMaterial(std::string matName);	

	void addAnimation(std::string name);
	void setSkeleton(std::string name);

	const std::string& getMaterial() const;

	// Returns mesh local position
	Buffers getBuffers() const;

	int getVertexCount() const { return m_vertexCount; }
	int getFaceCount() const { return m_faceCount; }
	std::string getName() const { return m_name; }
	const std::vector<std::string>& getAnimations() const { return m_animations; }
	const std::string& getSkeleton() const { return m_skeleton; }
	
	const std::vector<Face>& getFaces() { return m_faces; }
	const std::vector<Vertex>& getVertices() { return m_vertices; }
	const std::vector<Vertex2>& getVerticesSkele() { return m_skeleVertices; }
};

#endif