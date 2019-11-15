#ifndef _DSTRGENERATOR_h
#define _DSTRGENERATOR_h
//#include <Pch/Pch.h>
#include <GameObject/DestructibleObject.h>

class DstrGenerator
{
public:
	DstrGenerator();
	~DstrGenerator();

	void initPoints(glm::vec2 position = glm::vec2(), int amount = 0);
	void offsetPoints(glm::vec2 position = glm::vec2());

	void Destroy(DestructibleObject* object, glm::vec2 hitPosition);

	const unsigned int SeedRand(unsigned int seed = 0);
	const unsigned int GetSeed(unsigned int seed) const { return m_seed; }
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

	unsigned int m_seed;
	unsigned int m_breakPoints;
	float m_breakAreaRadius;

};

#endif
