#include <Pch/Pch.h>
#include "DestructibleObject.h"

DestructibleObject::DestructibleObject(DstrGenerator* dstr, int index, float fallGravity, float event1Time)
{
	dstrRef = dstr;
	m_type = DESTRUCTIBLE;
	m_scale = 0;
	m_polygonFace.resize(4);
	m_polygonUV.resize(4);
	m_index = index;

	m_ev1Time = event1Time;
	m_ev2Time = m_ev1Time;
	m_ev3Time = m_ev2Time;
	m_fallGravity = btVector3(0.0f, -fallGravity, 0.0f);
}

DestructibleObject::~DestructibleObject()
{
}

void DestructibleObject::update(float dt)
{
	m_lifetime += dt;
	updateBulletRigids();
	
	// Temporary variables to move later ---
	
	/*float dampingMidTime = 1.0f;
	float dampingSpinMidTime = 0.0f;
	float dampingAfterTime = 0.0f;
	float dampingSpinAfterTime = 0.0f;*/ // Stop-fall effect
	
	float dampingEv1 = 0.0f;
	float dampingSpinEv1 = 0.0f;
	float dampingEv2 = 0.0f;
	float dampingSpinEv2 = 0.0f;
	
	// Temporary variables to move later ---
	if (m_destroyed && m_dstrState != 3)
	{
	
		// Freezes object after time
		if (m_lifetime >= m_ev1Time && m_dstrState == 0)
		{
			for (int i = 0; i < (int)m_meshes.size(); i++)
			{
				if (m_meshes[i].body)
				{
					m_meshes[i].body->setGravity(m_fallGravity);
					m_meshes[i].body->setDamping(dampingEv1, dampingSpinEv1);
				}
			}
	
			m_dstrState = 1;
		}
	
		// Changes gravity after time
		if (m_lifetime >= m_ev2Time && m_dstrState == 1)
		{
			for (int i = 0; i < (int)m_meshes.size(); i++)
			{
				if (m_meshes[i].body)
				{
					m_meshes[i].body->setGravity(m_fallGravity);
					m_meshes[i].body->setDamping(dampingEv2, dampingSpinEv2);
				}
			}
	
			m_dstrState = 2;
		}
	
		// Removes the object after time
		if (m_lifetime >= 17.0f && m_dstrState == 2)
		{
			for (int i = 0; i < (int)m_meshes.size(); i++)
			{
				if (m_meshes[i].body)
				{
					removeBody(i);
				}
				setWorldPosition(glm::vec3(-999));
				m_dstrState = 3;
			}
		}
	}


}

void DestructibleObject::loadDestructible(std::string fileName, float size)
{
	BGLoader meshLoader;	// The file loader
	meshLoader.LoadMesh(MESHPATH + fileName);
	if (meshLoader.GetVertexCount() < 4)
	{
		logWarning("DSTR: Invalid destructible mesh: {0}", fileName);
		return;
	}

	findVertices(meshLoader.GetVertices());
	m_scale = size;
	meshFromPolygon(meshLoader.GetMeshName());
	
	// Load material
	Material newMaterial = meshLoader.GetMaterial();
	std::string materialName = newMaterial.name;
	if (!MaterialMap::getInstance()->existsWithName(materialName)) 	// This creates the material if it does not exist (by name)
	{
		if (meshLoader.GetAlbedo() != "-1")
		{
			std::string albedoFile = TEXTUREPATH + meshLoader.GetAlbedo();
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			// set the texture wrapping/filtering options (on the currently bound texture object)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// load and generate the texture
			int width, height, nrChannels;
			unsigned char* data = stbi_load(albedoFile.c_str(), &width, &height, &nrChannels, NULL);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				newMaterial.texture = true;
				newMaterial.textureID.push_back(texture);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);
		}
		else
		{
			newMaterial.texture = false;
		}

		setMaterial(MaterialMap::getInstance()->createMaterial(materialName, newMaterial));
		logTrace("Material created: {0}", materialName);
	}
	else
	{
		setMaterial(MaterialMap::getInstance()->getMaterial(materialName));
	}

	setTransform(meshLoader.GetTransform());
	meshLoader.Unload();
}

void DestructibleObject::loadDestructible(std::vector<Vertex> vertices_in, std::string name,
	Material newMaterial_in, std::string albedo_in, Transform transform, float size)
{
	findVertices(vertices_in);
	m_scale = size;
	meshFromPolygon(name);

	// Load material
	Material newMaterial = newMaterial_in;
	std::string materialName = newMaterial_in.name;
	if (!MaterialMap::getInstance()->existsWithName(materialName)) 	// This creates the material if it does not exist (by name)
	{
		if (albedo_in != "-1")
		{
			std::string albedoFile = TEXTUREPATH + albedo_in;
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			// set the texture wrapping/filtering options (on the currently bound texture object)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// load and generate the texture
			int width, height, nrChannels;
			unsigned char* data = stbi_load(albedoFile.c_str(), &width, &height, &nrChannels, NULL);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				newMaterial.texture = true;
				newMaterial.textureID.push_back(texture);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);

		}
		else
		{
			newMaterial.texture = false;
		}

		setMaterial(MaterialMap::getInstance()->createMaterial(materialName, newMaterial));
		logTrace("Material created: {0}", materialName);
	}
	else
	{
		setMaterial(MaterialMap::getInstance()->getMaterial(materialName));
	}


	setTransform(transform);
}

void DestructibleObject::loadBasic(std::string name)
{
	m_polygonFace[0] = glm::vec2(-1.0f, -2.0f);
	m_polygonFace[1] = glm::vec2(1.0f, -2.0f);
	m_polygonFace[2] = glm::vec2(1.0f, 8.0f);
	m_polygonFace[3] = glm::vec2(-1.0f, 8.0f);
	m_scale = 0.05f;

	int count = 0;
	count = m_polygonFace.size();

	std::vector<Vertex> newVertices;
	newVertices.resize(6 * count);
	std::vector<Face> newFace;
	newFace.resize(4 * count - 4);

	meshFromPolygon(name);
	initMesh(name, newVertices, newFace);
}

void DestructibleObject::loadDefined(std::string name, std::vector<glm::vec2> polygon)
{
	m_polygonFace = polygon;
	if (polygon.size() < 4)
	{
		logWarning("DSTR: Invalid destructible mesh: {0}", name);
		return;
	}
	m_scale = 0.05f;

	meshFromPolygon(name);
}

void DestructibleObject::findVertices(const std::vector<Vertex>& vertices)
{
	glm::vec3 normal = vertices[0].Normals;

	// Order of vertices
	int bL = 0;		// Bottom Left
	int bR = 1;		// Bottom Right
	int tR = 2;		// Top Right
	int tL = 3;		// Top Left

	enum NormalDirection
	{
		undefined,
		Xpositive, Ypositive, Zpositive,
		Xnegative, Ynegative, Znegative
	};
	NormalDirection ndir = undefined;

	int x = glm::normalize(normal).x;
	int y = glm::normalize(normal).y;
	int z = glm::normalize(normal).z;
	if (x == 1 && y == 0 && z == 0)
		ndir = Xpositive;
	else if (x == 0 && y == 1 && z == 0)
		ndir = Ypositive;
	else if (x == 0 && y == 0 && z == 1)
		ndir = Zpositive;
	else if (x == -1 && y == 0 && z == 0)
		ndir = Xnegative;
	else if (x == 0 && y == -1 && z == 0)
		ndir = Ynegative;
	else if (x == 0 && y == 0 && z == -1)
		ndir = Znegative;

	switch (ndir)
	{
	case (Xpositive):
		for (size_t i = 0; i < 4; i++)
		{
			if (vertices[i].position.z > 0 && vertices[i].position.y < 0)
				bL = i;
			if (vertices[i].position.z < 0 && vertices[i].position.y < 0)
				bR = i;
			if (vertices[i].position.z < 0 && vertices[i].position.y > 0)
				tR = i;
			if (vertices[i].position.z > 0 && vertices[i].position.y > 0)
				tL = i;
		}
		break;
	case (Ypositive):
		for (size_t i = 0; i < 4; i++)
		{
			if (vertices[i].position.z > 0 && vertices[i].position.x < 0)
				bL = i;
			if (vertices[i].position.z > 0 && vertices[i].position.x > 0)
				bR = i;
			if (vertices[i].position.z < 0 && vertices[i].position.x > 0)
				tR = i;
			if (vertices[i].position.z < 0 && vertices[i].position.x < 0)
				tL = i;
		}
		break;
	case (Zpositive):
		for (size_t i = 0; i < 4; i++)
		{
			if (vertices[i].position.x < 0 && vertices[i].position.y < 0)
				bL = i;
			if (vertices[i].position.x > 0 && vertices[i].position.y < 0)
				bR = i;
			if (vertices[i].position.x > 0 && vertices[i].position.y > 0)
				tR = i;
			if (vertices[i].position.x < 0 && vertices[i].position.y > 0)
				tL = i;
		}
		break;
	case (Xnegative):
		for (size_t i = 0; i < 4; i++)
		{
			if (vertices[i].position.z < 0 && vertices[i].position.y < 0)
				bL = i;
			if (vertices[i].position.z > 0 && vertices[i].position.y < 0)
				bR = i;
			if (vertices[i].position.z > 0 && vertices[i].position.y > 0)
				tR = i;
			if (vertices[i].position.z < 0 && vertices[i].position.y > 0)
				tL = i;
		}
		break;
	case (Ynegative):
		for (size_t i = 0; i < 4; i++)
		{
			if (vertices[i].position.x < 0 && vertices[i].position.z < 0)
				bL = i;
			if (vertices[i].position.x > 0 && vertices[i].position.z < 0)
				bR = i;
			if (vertices[i].position.x > 0 && vertices[i].position.z > 0)
				tR = i;
			if (vertices[i].position.x < 0 && vertices[i].position.z > 0)
				tL = i;
		}
		break;
	case (Znegative):
		for (size_t i = 0; i < 4; i++)
		{
			if (vertices[i].position.x > 0 && vertices[i].position.y < 0)
				bL = i;
			if (vertices[i].position.x < 0 && vertices[i].position.y < 0)
				bR = i;
			if (vertices[i].position.x < 0 && vertices[i].position.y > 0)
				tR = i;
			if (vertices[i].position.x > 0 && vertices[i].position.y > 0)
				tL = i;
		}
		break;
	default:
		logWarning("DSTR: Error finding vertices of destructible mesh, using default");
		break;
	}


	m_polygonFace[0] = vertices[bL].position;	// Bottom Left
	m_polygonFace[1] = vertices[bR].position;	// Bottom Right
	m_polygonFace[2] = vertices[tR].position;	// Top Right
	m_polygonFace[3] = vertices[tL].position;	// Top Left

	m_polygonUV[0] = vertices[bL].UV;
	m_polygonUV[1] = vertices[bR].UV;
	m_polygonUV[2] = vertices[tR].UV;
	m_polygonUV[3] = vertices[tL].UV;
}

void DestructibleObject::meshFromPolygon(std::string name)
{
	int count = 0;
	glm::vec3 normal = glm::vec3();

	count = m_polygonFace.size();

	std::vector<Vertex> newVertices;
	newVertices.resize(6 * count);
	std::vector<Face> newFace;
	newFace.resize(4 * count - 4);

	int vi = 0;
	int uvi = 0;
	int ni = 0;
	int ti = 0;

	// Front
	for (int i = 0; i < count; i++)
	{
		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, 0);
		newVertices[uvi++].UV = glm::vec2(m_polygonUV[i].x, m_polygonUV[i].y);
		newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	// Back
	for (int i = 0; i < count; i++)
	{
		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, -m_scale);
		newVertices[uvi++].UV = glm::vec2(m_polygonUV[i].x, m_polygonUV[i].y);
		newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, -1.0f);
	}

	// Sides
	for (int i = 0; i < count; i++)
	{
		int iNext = i == count - 1 ? 0 : i + 1;

		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, 0);
		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, -m_scale);
		newVertices[vi++].position = glm::vec3(m_polygonFace[iNext].x, m_polygonFace[iNext].y, -m_scale);
		newVertices[vi++].position = glm::vec3(m_polygonFace[iNext].x, m_polygonFace[iNext].y, 0);

		normal = glm::normalize(glm::cross(glm::vec3(m_polygonFace[iNext] - m_polygonFace[i], 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

		newVertices[ni++].Normals = normal;
		newVertices[ni++].Normals = normal;
		newVertices[ni++].Normals = normal;
		newVertices[ni++].Normals = normal;
	}

	for (int vert = 2; vert < count; vert++)
	{
		newFace[ti].indices[0] = 0;
		newFace[ti].indices[1] = vert - 1;
		newFace[ti].indices[2] = vert;
		ti++;
	}

	for (int vert = 2; vert < count; vert++)
	{
		newFace[ti].indices[0] = count;
		newFace[ti].indices[1] = count + vert;
		newFace[ti].indices[2] = count + vert - 1;
		ti++;
	}

	int si = 0;
	for (int vert = 0; vert < count; vert++)
	{
		si = 2 * count + 4 * vert;

		newFace[ti].indices[0] = si;
		newFace[ti].indices[1] = si + 1;
		newFace[ti].indices[2] = si + 2;
		ti++;

		newFace[ti].indices[0] = si;
		newFace[ti].indices[1] = si + 2;
		newFace[ti].indices[2] = si + 3;
		ti++;
	}

	initMesh(name, newVertices, newFace);

	// TODO: Maybe move or fix this better
	Transform t = getTransform();
	t.position.z += m_scale * 0.5;
	setMeshOffsetTransform(t);
}
