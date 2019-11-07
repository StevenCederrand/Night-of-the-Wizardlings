#ifndef _POINTCOMPARE_h
#define _POINTCOMPARE_h
#include <Pch/Pch.h>


class PointCompare
{
public:

	PointCompare();
	PointCompare(std::vector<int> tris, std::vector<glm::vec2> verts);
	bool Compare(PointTriangle const& pt0, PointTriangle const& pt1);
	bool CompareAngles(PointTriangle const& pt0, PointTriangle const& pt1);
	glm::vec2 Centroid(PointTriangle const& pt);

	void SetTris(std::vector<int> tris);
	void SetVerts(std::vector<glm::vec2> verts);

	void ClearTris(std::vector<glm::vec2> verts);
	void ClearVerts(std::vector<glm::vec2> verts);



private:
	std::vector<int> m_tris;
	std::vector<glm::vec2> m_verts;

	Geometry geomtry;

};


#endif