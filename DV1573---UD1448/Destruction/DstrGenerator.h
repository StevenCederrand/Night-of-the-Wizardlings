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

	void Destroy(DestructibleObject* object, glm::vec2 hitPosition = glm::vec3(0.0f), glm::vec3 hitDirection = glm::vec3(0.0f));

	const unsigned int seedRand(unsigned int seed = 0);
	const unsigned int getSeed(unsigned int seed) const { return m_seed; }
	const std::vector<DestructionPacket>& getPackets() const { return m_packets; }

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


	std::vector<DestructionPacket> m_packets;
	unsigned int m_seed;
	unsigned int m_breakPoints;
	float m_breakAreaRadius;

};

#endif
