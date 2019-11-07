#ifndef _TRIANGLENODE_h
#define _TRIANGLENODE_h
#include <Pch/Pch.h>

// A single node in the triangle tree.
// All parameters are indexes.
// negative numbers means it doesn't exist or isn't assigned
struct TriangleNode
{
	int m_vertex_0;
	int m_vertex_1;
	int m_vertex_2;
	
	// Child triangle
	int m_cvertex_0;
	int m_cvertex_1;
	int m_cvertex_2;
	
	// Adjacent triangle (vertex_1 & vertex_2 as an edge).
	int m_avertex_0;
	int m_avertex_1;
	int m_avertex_2;

	TriangleNode(int vertex_0, int vertex_1, int vertex_2)
	{
		m_vertex_0 = vertex_0;
		m_vertex_1 = vertex_1;
		m_vertex_2 = vertex_2;

		m_cvertex_0 = -1;
		m_cvertex_1 = -1;
		m_cvertex_2 = -1;

		m_avertex_0 = -1;
		m_avertex_1 = -1;
		m_avertex_2 = -1;
	}

	bool Is_Leaf() { return m_cvertex_0 < 0 && m_cvertex_1 < 0 && m_cvertex_2 < 0; }
	bool Is_Inner() { return m_vertex_0 >= 0 && m_vertex_1 >= 0 && m_vertex_2 >= 0; }

	bool Has_Edge(int edge_0, int edge_1)
	{ 
		if (edge_0 == m_vertex_0)
			return edge_1 == m_vertex_1 || edge_1 == m_vertex_2;
		else if(edge_0 == m_vertex_1)
			return edge_1 == m_vertex_0 || edge_1 == m_vertex_2;
		else if (edge_0 == m_vertex_2)
			return edge_1 == m_vertex_0 || edge_1 == m_vertex_1;

		return false; 
	}

	int GetLastPoint(int vertex_0, int vertex_1)
	{
		if (vertex_0 == m_vertex_0)
		{
			if (vertex_1 == m_vertex_1) return m_vertex_2;
			if (vertex_1 == m_vertex_2) return m_vertex_1;
		}
		if (vertex_0 == m_vertex_1)
		{
			if (vertex_1 == m_vertex_0) return m_vertex_2;
			if (vertex_1 == m_vertex_2) return m_vertex_0;
		}
		if (vertex_0 == m_vertex_2)
		{
			if (vertex_1 == m_vertex_0) return m_vertex_1;
			if (vertex_1 == m_vertex_1) return m_vertex_0;
		}
	}

	int GetOpposite(int vertex)
	{
		if (vertex == m_vertex_0) return m_avertex_0;
		if (vertex == m_vertex_1) return m_avertex_1;
		if (vertex == m_vertex_2) return m_avertex_2;
	}





};


#endif

