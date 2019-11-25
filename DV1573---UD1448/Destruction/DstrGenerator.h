#ifndef _DSTRGENERATOR_h
#define _DSTRGENERATOR_h
//#include <Pch/Pch.h>
#include <GameObject/DestructibleObject.h>

enum DSTRType
{
	DSTR1,
	DSTR2,
	DSTR3,
	DSTR4
};

class DstrGenerator
{
public:
	DstrGenerator();
	~DstrGenerator();

	void initPoints(glm::vec2 position = glm::vec2());
	void setBreakSettings(DSTRType type, float breakPoints, float breakAreaRadius, float gravity);
	void setBreakPoints(float breakPoints);
	void setBreakRadius(float breakAreaRadius);
	void setBreakGravity(float gravity);
	void setBreakType(DSTRType type);

	void offsetPoints(glm::vec2 position = glm::vec2());

	void Destroy(DestructibleObject* object, glm::vec2 hitPosition = glm::vec3(0.0f), glm::vec3 hitDirection = glm::vec3(0.0f));

	const unsigned int seedRand(int seed = -1);
	const unsigned int getSeed(unsigned int seed) const { return m_seed; }

	void Clear();

private:
	VoroniCalculator m_voroniCalc;
	VoroniDiagram m_diagram;
	VoroniClipper m_clipper;
	std::vector<glm::vec2> m_clipped;
	std::vector<glm::vec2> m_randomPoints;
	std::vector<Vertex> m_newVertices;
	std::vector<Face> m_newFace;

	std::vector<Mesh> m_meshResults;
	
	int m_seed;

	DSTRType m_dstType = DSTR1;
	unsigned int m_breakPoints;
	float m_breakAreaRadius;
	btVector3 m_initGravity;

};

#endif
