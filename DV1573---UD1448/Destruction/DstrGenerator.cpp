#include <Pch/Pch.h>
#include <Destruction/DstrGenerator.h>

DstrGenerator::DstrGenerator()
{
	m_seed = 0;
	m_breakPoints = 16;
	m_breakAreaRadius = 1.8f;
	m_initGravity = btVector3(0.0f, -50.0f, 0.0f);

	//m_breakPoints = 22;
	//m_breakAreaRadius = 18.8f;

	m_randomPoints.resize(m_breakPoints);
	initPoints();
}

DstrGenerator::~DstrGenerator()
{
}

void DstrGenerator::initPoints(glm::vec2 position)
{
	//if (amount && amount != m_breakPoints)
	//{
	//	m_breakPoints = amount;
	m_randomPoints.resize(m_breakPoints);
	//}
	//else if (position != glm::vec2(0))
	//{
	//	offsetPoints();
	//	return;
	//}

	float rnd = 0;
	float offset = 0;
	float angle = 0;

	seedRand(); // Important for the networking to get consistent breakage

	if (m_breakAreaRadius < 0.1f)
	{
		m_breakAreaRadius = 0.1f;
		logWarning("Not enough precision for break radius less than 0.1, clamping");
	}

	switch (m_dstType)
	{

	// Creates points randomly
	case DSTR2:
		for (int i = 0; i < m_randomPoints.size(); i++)
		{
			rnd = rand() % 1000 + 1;
			rnd /= 1000;
			offset = m_breakAreaRadius;
			offset *= glm::sqrt(rnd);

			rnd = rand() % 1000 + 1;
			rnd /= 1000;
			angle = 2.0f * glm::pi<float>() * rnd;

			m_randomPoints[i] = position;
			m_randomPoints[i].x += offset * glm::cos(angle);
			m_randomPoints[i].y += offset * glm::sin(angle);
		}
		break;

		// Creates points in a fine circle with an outer circle
	default:
		// Creates points in a fine circle
		m_randomPoints[0] = position;
		for (int i = 1; i < m_randomPoints.size(); i++)
		{
			rnd = rand() % 1000 + 1;
			rnd /= 1000;
			offset = m_breakAreaRadius;
			//offset += (0.05f * m_breakAreaRadius * glm::sqrt(rnd));

			rnd = rand() % 1000 + 1;
			rnd /= 1000;
			angle = 2.0f * glm::pi<float>() * rnd;

			m_randomPoints[i] = position;
			m_randomPoints[i].x += offset * glm::cos(angle);
			m_randomPoints[i].y += offset * glm::sin(angle);
		}

		// Creates points in a fine circle slighly larger
		for (int i = m_randomPoints.size() / 2; i < m_randomPoints.size(); i++)
		{
			rnd = rand() % 1000 + 1;
			rnd /= 1000;
			offset = m_breakAreaRadius * 1.5;
			offset += (0.05f * m_breakAreaRadius * glm::sqrt(rnd));

			rnd = rand() % 1000 + 1;
			rnd /= 1000;
			angle = 2.0f * glm::pi<float>() * rnd;

			m_randomPoints[i] = position;
			m_randomPoints[i].x += offset * glm::cos(angle);
			m_randomPoints[i].y += offset * glm::sin(angle);
		}
		break;

	}
	
}

void DstrGenerator::setBreakSettings(DSTRType type, float breakPoints, float breakAreaRadius, float gravity)
{
	m_dstType = type;
	m_breakPoints = breakPoints;
	m_breakAreaRadius = breakAreaRadius;
	m_initGravity = btVector3(0.0f, -gravity, 0.0f);
}

void DstrGenerator::setBreakPoints(float breakPoints)
{
	m_breakPoints = breakPoints;
}

void DstrGenerator::setBreakRadius(float breakAreaRadius)
{
	m_breakAreaRadius = breakAreaRadius;
}

void DstrGenerator::setBreakGravity(float gravity)
{
	m_initGravity = btVector3(0.0f, -gravity, 0.0f);
}

void DstrGenerator::setBreakType(DSTRType type)
{
	m_dstType = type;
}

void DstrGenerator::offsetPoints(glm::vec2 position)
{
	for (int i = 0; i < m_randomPoints.size(); i++)
	{
		m_randomPoints[i] += position;
	}
}

void DstrGenerator::update()
{
	m_timeSinceLastDestruction += DeltaTime;
}

void DstrGenerator::Destroy(DestructibleObject* object, glm::vec2 hitPosition, glm::vec3 hitDirection)
{
	if (object->is_destroyed())
		return;	

	if (m_timeSinceLastDestruction > 0.3f)
	{
		int slot = SoundHandler::getInstance()->playSound(DestructionSound, Client::getInstance()->getMyData().guid);
		SoundHandler::getInstance()->setSourcePosition(object->getTransform().position, DestructionSound, Client::getInstance()->getMyData().guid, slot);
		m_timeSinceLastDestruction = 0.0f;
	}

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
			
			// Top
			for (int i = 0; i < count; i++)
			{
				m_newVertices[vi++].position = glm::vec3(m_clipped[i].x, m_clipped[i].y, scale);
				m_newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, 1.0f);
			}

			// Bottom
			for (int i = 0; i < count; i++)
			{
				m_newVertices[vi++].position = glm::vec3(m_clipped[i].x, m_clipped[i].y, -scale);
				m_newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, -1.0f);
			}

			// Sides
			for (int i = 0; i < count; i++)
			{
				int iNext = i == count - 1 ? 0 : i + 1;

				m_newVertices[vi++].position = glm::vec3(m_clipped[i].x, m_clipped[i].y, scale);
				m_newVertices[vi++].position = glm::vec3(m_clipped[i].x, m_clipped[i].y, -scale);
				m_newVertices[vi++].position = glm::vec3(m_clipped[iNext].x, m_clipped[iNext].y, -scale);
				m_newVertices[vi++].position = glm::vec3(m_clipped[iNext].x, m_clipped[iNext].y, scale);

				normal = glm::normalize(glm::cross(glm::vec3(m_clipped[iNext] - m_clipped[i], 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

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


			glm::vec3 min = m_newVertices[0].position;
			glm::vec3 max = m_newVertices[0].position;
			for (size_t i = 0; i < m_newVertices.size(); i++)
			{
				min.x = fminf(m_newVertices[i].position.x, min.x);
				min.y = fminf(m_newVertices[i].position.y, min.y);
				min.z = fminf(m_newVertices[i].position.z, min.z);

				max.x = fmaxf(m_newVertices[i].position.x, max.x);
				max.y = fmaxf(m_newVertices[i].position.y, max.y);
				max.z = fmaxf(m_newVertices[i].position.z, max.z);
			}
			glm::vec3 center = glm::vec3((min + max) * 0.5f);

			for (size_t i = 0; i < m_newVertices.size(); i++)
			{
				m_newVertices[i].position -= center;
			}

			object->initMesh(object->getMesh()->getName() + "_" + std::to_string(i), m_newVertices, m_newFace);

			Transform newTransform = object->getTransform();
			newTransform.position += center * glm::inverse(newTransform.rotation);;
			object->setTransform(newTransform, mi);

			// DEBUG PLACEMENT
			//Transform newTransform = object->getTransform(0);
			//newTransform.position += glm::vec3(
			//	m_diagram.sites[i].x * 0.2f,
			//	m_diagram.sites[i].y * 0.2f,
			//	0.0f);
			//object->setTransform(newTransform,  mi);

			object->createDynamic(CollisionObject::box, 100.0f * scale, mi, true);
			
			// Values for destroyed object
			// TODO: Move thiss
			glm::vec3 forceDir = glm::vec3((m_diagram.sites[i] - hitPosition), 0.0f) * glm::inverse(newTransform.rotation);
			glm::vec3 force = (forceDir * 20) + hitDirection;
			if (scale > 0.6)
				force = (forceDir * 100) + hitDirection * 9;	//Hardcoded for pillar, to fix later pls dont worrying

			if (scale > 1.5)
				force = (forceDir * 200) + hitDirection * 30;	//Hardcoded for big walls, to fix later pls dont worrying

			float forceRand = rand() % 300 + 900;
			forceRand /= 1000;
			force *= forceRand;

			float dirRndX = 2 * (double)rand() / (double)RAND_MAX - 1;
			float dirRndY = 2 * (double)rand() / (double)RAND_MAX - 1;
			float dirRndZ = 2 * (double)rand() / (double)RAND_MAX - 1;
			
			btRigidBody* body = object->getRigidBody(mi);
			body->applyCentralImpulse(btVector3(force.x, force.y, force.z) * 1.4);
			body->applyTorque(btVector3(force.x * dirRndX, force.y * dirRndY, force.z * dirRndZ) * 15);
			body->setGravity(m_initGravity);

			mi++;
		}
	}

	object->set_BtActive(false);
	object->set_destroyed(true);
	object->setLifetime(0.0f);

	object->setMaterial(nullptr, -1);
	object->setBTWorldPosition(glm::vec3(-999.0f), 0);
	object->setWorldPosition(glm::vec3(-999.0f), 0);
}

const unsigned int DstrGenerator::seedRand(int seed)
{
	if (seed == -1)
	{
		m_seed = int(time(NULL));
		srand(m_seed);
	}
	else
	{
		srand(seed);
		return seed;
	}

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


