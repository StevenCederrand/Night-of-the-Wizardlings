#include <Pch/Pch.h>
#include <Destruction/DstrGenerator.h>

DstrGenerator::DstrGenerator()
{
	m_seed = 0;
	m_breakPoints = 15;
	m_breakAreaRadius = 1.6f;
	m_randomPoints.resize(m_breakPoints);
	initPoints();
}

DstrGenerator::~DstrGenerator()
{
}

void DstrGenerator::initPoints(glm::vec2 position, int amount)
{
	if (amount && amount != m_breakPoints)
	{
		m_breakPoints = amount;
		m_randomPoints.resize(m_breakPoints);
	}
	//else if (position != glm::vec2(0))
	//{
	//	offsetPoints();
	//	return;
	//}

	float rnd = 0;
	float offset = 0;
	float angle = 0;

	SeedRand(); // Important for the networking to get consistent breakage

	if (m_breakAreaRadius < 0.1f)
	{
		m_breakAreaRadius = 0.1f;
		logWarning("Not enough precision for break radius less than 0.1, clamping");
	}

	m_randomPoints[0] = position;
	for (int i = 1; i < m_randomPoints.size(); i++)
	{
		rnd = rand() % 999 + 1;
		rnd /= 1000;
		offset = 1.5f;
		offset += (0.01f * m_breakAreaRadius * glm::sqrt(rnd));

		rnd = rand() % 999 + 1;
		rnd /= 1000;
		angle = 2.0f * glm::pi<float>() * rnd;
	
		m_randomPoints[i] = position;
		m_randomPoints[i].x += offset * glm::cos(angle);
		m_randomPoints[i].y += offset * glm::sin(angle);
	}

	//for (int i = m_randomPoints.size() / 2; i < m_randomPoints.size(); i++)
	//{
	//	rnd = rand() % 999 + 1;
	//	rnd /= 1000;
	//	offset = 0.6f;
	//	//offset += (0.01f * m_breakAreaRadius * glm::sqrt(rnd));
	//
	//	rnd = rand() % 999 + 1;
	//	rnd /= 1000;
	//	angle = 2.0f * glm::pi<float>() * rnd;
	//
	//	m_randomPoints[i] = position;
	//	m_randomPoints[i].x += offset * glm::cos(angle);
	//	m_randomPoints[i].y += offset * glm::sin(angle);
	//}
}

void DstrGenerator::offsetPoints(glm::vec2 position)
{
	for (int i = 0; i < m_randomPoints.size(); i++)
	{
		m_randomPoints[i] += position;
	}
}

void DstrGenerator::Destroy(DestructibleObject* object, glm::vec2 hitPosition)
{
	if (object->is_destroyed())
		return;

	Clear();

	initPoints(hitPosition);

	m_diagram = m_voroniCalc.CalculateDiagram(m_randomPoints);
	m_diagram.sites = m_diagram.triangulation.vertices;

	const std::vector<glm::vec2> polygon = object->getPolygon();
	float scale = object->getScale();

	int count = 0;
	int vi = 0;
	int ni = 0;
	int ti = 0;
	int mi = 1; // The first mesh (mi == 0) is the initial mesh to destroy
	glm::vec3 normal = glm::vec3();

	for (int i = 0; i < m_randomPoints.size(); i++)
	{
		m_clipper.ClipSite(m_diagram, polygon, i, m_clipped);

		if (m_clipped.size() > 0)
		{
			count = m_clipped.size();
			//TODO: one point was nan(ind)
			
			m_newVertices.clear();
			m_newVertices.resize(6 * count);

			m_newFace.clear();
			m_newFace.resize(4 * count - 4);

			vi = 0;
			ni = 0;
			ti = 0;

			glm::vec3 min = glm::vec3(m_clipped[0], 0.0f);
			glm::vec3 max = glm::vec3(m_clipped[0], 0.0f);
			
			// Top
			for (int i = 0; i < count; i++)
			{
				min.x = fminf(m_newVertices[vi].position.x, min.x);
				min.y = fminf(m_newVertices[vi].position.y, min.y);
				max.x = fmaxf(m_newVertices[vi].position.x, max.x);
				max.y = fmaxf(m_newVertices[vi].position.y, max.y);

				m_newVertices[vi++].position = glm::vec3(m_clipped[i].x, m_clipped[i].y, scale);
				m_newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, -1.0f);

			}

			// Bottom
			for (int i = 0; i < count; i++)
			{
				min.x = fminf(m_newVertices[vi].position.x, min.x);
				min.y = fminf(m_newVertices[vi].position.y, min.y);
				max.x = fmaxf(m_newVertices[vi].position.x, max.x);
				max.y = fmaxf(m_newVertices[vi].position.y, max.y);

				m_newVertices[vi++].position = glm::vec3(m_clipped[i].x, m_clipped[i].y, -scale);
				m_newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, 1.0f);
			}

			// Sides
			for (int i = 0; i < count; i++)
			{
				int iNext = i == count - 1 ? 0 : i + 1;

				min.x = fminf(m_newVertices[vi].position.x, min.x);
				min.y = fminf(m_newVertices[vi].position.y, min.y);
				max.x = fmaxf(m_newVertices[vi].position.x, max.x);
				max.y = fmaxf(m_newVertices[vi].position.y, max.y);

				m_newVertices[vi++].position = glm::vec3(m_clipped[i].x, m_clipped[i].y, scale);
				m_newVertices[vi++].position = glm::vec3(m_clipped[i].x, m_clipped[i].y, -scale);
				m_newVertices[vi++].position = glm::vec3(m_clipped[iNext].x, m_clipped[iNext].y, -scale);
				m_newVertices[vi++].position = glm::vec3(m_clipped[iNext].x, m_clipped[iNext].y, scale);

				normal = glm::normalize(glm::cross(glm::vec3(m_clipped[iNext] - m_clipped[i], 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));

				m_newVertices[ni++].Normals = normal;
				m_newVertices[ni++].Normals = normal;
				m_newVertices[ni++].Normals = normal;
				m_newVertices[ni++].Normals = normal;
			}

			for (int vert = 2; vert < count; vert++)
			{
				m_newFace[ti].indices[0] = 0;
				m_newFace[ti].indices[1] = vert - 1;
				m_newFace[ti].indices[2] = vert;
				ti++;
			}

			for (int vert = 2; vert < count; vert++)
			{
				m_newFace[ti].indices[0] = count;
				m_newFace[ti].indices[1] = count + vert;
				m_newFace[ti].indices[2] = count + vert - 1;
				ti++;
			}

			int si = 0;
			for (int vert = 0; vert < count; vert++)
			{
				si = 2 * count + 4 * vert;

				m_newFace[ti].indices[0] = si;
				m_newFace[ti].indices[1] = si + 1;
				m_newFace[ti].indices[2] = si + 2;
				ti++;

				m_newFace[ti].indices[0] = si;
				m_newFace[ti].indices[1] = si + 2;
				m_newFace[ti].indices[2] = si + 3;
				ti++;
			}

			glm::vec3 center = glm::vec3((min + max) * 0.5f);

			//for (int j = 0; j < m_newVertices.size(); j++)
			//{
			//	m_newVertices[j].position -= center;
			//}

			object->initMesh(object->getMeshName() + "_" + std::to_string(i), m_newVertices, m_newFace);

			Transform newTransform = object->getTransform();
			newTransform.position += center;
			object->setTransform(newTransform, mi);

			//Transform newTransform = object->getTransform(0);
			//newTransform.position += glm::vec3(
			//	m_diagram.sites[i].x * 0.2f,
			//	m_diagram.sites[i].y * 0.2f,
			//	0.0f);
			//object->setTransform(newTransform,  mi);

			object->createDynamicRigidBody(CollisionObject::box, NULL, 20.0f, mi, false);
			mi++;

		}
	}

	object->setBTWorldPosition(glm::vec3(-99.0f), 0);
	object->setWorldPosition(glm::vec3(-99.0f), 0);
	object->set_BtActive(false, 0);
	//object->createDynamicRigidBody(CollisionObject::box, NULL, 200.0f, 1);
	//object->set_BtActive(false, 0);
	object->set_destroyed(true);
}

const unsigned int DstrGenerator::SeedRand(unsigned int seed)
{
	m_seed = unsigned int(time(NULL));
	srand(m_seed);
	return m_seed;
}

void DstrGenerator::Clear()
{
	m_diagram.Clear();

	m_clipped.clear();
	m_clipped.shrink_to_fit();

	m_newVertices.clear();
	m_newVertices.shrink_to_fit();
	m_newFace.clear();
	m_newFace.shrink_to_fit();

	m_meshResults.clear();
	m_meshResults.shrink_to_fit();

}


