#ifndef _DELAUNAYTRIANGULATION_h
#define _DELAUNAYTRIANGULATION_h
#include <Pch/Pch.h>


class DelaunayTriangulation
{
public:
	DelaunayTriangulation();
	~DelaunayTriangulation();

	void Triangulate(std::vector<glm::vec2> points);
	bool Higher(int index_0, int index_1);
	void BoyerWatson();



private:
	std::vector<glm::vec2> m_vertices;
	std::vector<int> m_indices;
	std::vector<TriangleNode> m_triangles;

	int highest;

};

#endif
