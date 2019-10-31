#include "Pch/Pch.h"
#include "DelaunayTriangulation.h"

DelaunayTriangulation::DelaunayTriangulation()
{
}

DelaunayTriangulation::~DelaunayTriangulation()
{
}

void DelaunayTriangulation::Triangulate(std::vector<glm::vec2> points)
{
	m_vertices = points;
	highest = 0;

	for (size_t i = 0; i < m_vertices.size(); i++)
	{
		if (Higher(highest, i))
		{
			highest = i;
		}
	}

	m_triangles.push_back(TriangleNode(-2, -1, highest));


}

bool DelaunayTriangulation::Higher(int index_0, int index_1)
{

	glm::vec2 vertex_0 = m_vertices[index_0];
	glm::vec2 vertex_1 = m_vertices[index_1];

	if (vertex_0.y < vertex_1.y)
	{
		return true;
	}
	else if (vertex_0.y > vertex_1.y)
	{
		return false;
	}
	else
	{
		return vertex_0.x < vertex_1.x;
	}


}

void DelaunayTriangulation::BoyerWatson()
{
}
