#include <Pch/Pch.h>
#include "DestructibleObject.h"

DestructibleObject::DestructibleObject(DstrGenerator* dstr, int index, float fallTime, float fallGravity)
{
	dstrRef = dstr;
	m_type = DESTRUCTIBLE;
	m_scale = 0;
	m_polygonFace.reserve(4);
	m_index = index;

	m_fallTime = fallTime;
	m_fallGravity = btVector3(0.0f, -fallGravity, 0.0f);
}

DestructibleObject::~DestructibleObject()
{
}

void DestructibleObject::update(float dt)
{
	updateBulletRigids();
	

	if (m_destroyed && m_dstrState != 3)
	{
		m_lifetime += dt;

		// Freezes object after time
		if (m_lifetime >= m_fallTime * 0.60f && m_dstrState == 0)
		{
			for (int i = 0; i < (int)getRigidBodies().size(); i++)
			{
				//getRigidBodies()[i]->setGravity(m_fallGravity);
				//getRigidBodies()[i]->setDamping(0.85f, 0.0f);
			}

			m_dstrState = 1;
		}

		// Changes gravity after time
		if (m_lifetime >= m_fallTime && m_dstrState == 1)
		{
			for (int i = 0; i < (int)getRigidBodies().size(); i++)
			{
				getRigidBodies()[i]->setGravity(m_fallGravity / 8);
				getRigidBodies()[i]->setDamping(0.0f, 0.0f);
			}

			m_dstrState = 2;
		}

		// Removes the object after time
		if (m_lifetime >= 15.0f && m_dstrState == 2)
		{
			for (int i = 0; i < (int)getRigidBodies().size(); i++)
			{
				removeBody(i);
				setWorldPosition(glm::vec3(-999));
				m_dstrState = 3;
				//getRigidBodies()[i]->setActivationState(false);
				//getRigidBodies()[i]->forceActivationState(false);
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

	std::vector<Vertex> vertices;
	vertices.resize(4);
	vertices[0] = meshLoader.GetVertices()[0];
	vertices[0].position.z = 0.0f;
	vertices[1] = meshLoader.GetVertices()[1];
	vertices[1].position.z = 0.0f;
	vertices[2] = meshLoader.GetVertices()[3];
	vertices[2].position.z = 0.0f;
	vertices[3] = meshLoader.GetVertices()[2];
	vertices[3].position.z = 0.0f;

	m_polygonFace.resize(4);
	m_polygonFace[0] = vertices[0].position;
	m_polygonFace[1] = vertices[1].position;
	m_polygonFace[2] = vertices[2].position;
	m_polygonFace[3] = vertices[3].position;
	m_scale = size;

	meshFromPolygon(meshLoader.GetMeshName());
	
	// Load material
	Material newMaterial = meshLoader.GetMaterial();
	std::string materialName = newMaterial.name;
	setMaterial(materialName);
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

		MaterialMap::getInstance()->createMaterial(materialName, newMaterial);
		logTrace("Material created: {0}", materialName);
	}


	setTransform(meshLoader.GetTransform());
	meshLoader.Unload();
}

void DestructibleObject::loadDestructible(std::vector<Vertex> vertices_in, std::string name,
	Material newMaterial_in, std::string albedo_in, Transform transform, float size)
{
	std::vector<Vertex> vertices;
	vertices.resize(4);
	vertices[0] = vertices_in[0];
	vertices[0].position.z = 0.0f;
	vertices[1] = vertices_in[1];
	vertices[1].position.z = 0.0f;
	vertices[2] = vertices_in[3];
	vertices[2].position.z = 0.0f;
	vertices[3] = vertices_in[2];
	vertices[3].position.z = 0.0f;

	m_polygonFace.resize(4);
	m_polygonFace[0] = vertices[0].position;
	m_polygonFace[1] = vertices[1].position;
	m_polygonFace[2] = vertices[2].position;
	m_polygonFace[3] = vertices[3].position;
	m_scale = size;

	meshFromPolygon(name);

	// Load material
	Material newMaterial = newMaterial_in;
	std::string materialName = newMaterial_in.name;
	setMaterial(materialName);
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

		MaterialMap::getInstance()->createMaterial(materialName, newMaterial);
		logTrace("Material created: {0}", materialName);
	}


	setTransform(transform);
}

void DestructibleObject::loadBasic(std::string name)
{
	m_polygonFace.resize(4);
	m_polygonFace[0] = glm::vec2(-1.0f, -2.0f);
	m_polygonFace[1] = glm::vec2(1.0f, -2.0f);
	m_polygonFace[2] = glm::vec2(1.0f, 8.0f);
	m_polygonFace[3] = glm::vec2(-1.0f, 8.0f);
	m_scale = 0.05f;

	int count = 0;
	glm::vec3 normal = glm::vec3();

	count = m_polygonFace.size();

	std::vector<Vertex> newVertices;
	newVertices.resize(6 * count);
	std::vector<Face> newFace;
	newFace.resize(4 * count - 4);


	int vi = 0;
	int ni = 0;
	int ti = 0;
	m_scale = 1.0f;
	float scale = m_scale;

	// Top
	for (int i = 0; i < count; i++)
	{
		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, scale);
		newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, -1.0f);
	}

	// Bottom
	for (int i = 0; i < count; i++)
	{
		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, -scale);
		newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	// Sides
	for (int i = 0; i < count; i++)
	{
		int iNext = i == count - 1 ? 0 : i + 1;

		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, scale);
		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, -scale);
		newVertices[vi++].position = glm::vec3(m_polygonFace[iNext].x, m_polygonFace[iNext].y, -scale);
		newVertices[vi++].position = glm::vec3(m_polygonFace[iNext].x, m_polygonFace[iNext].y, scale);

		normal = glm::normalize(glm::cross(glm::vec3(m_polygonFace[iNext] - m_polygonFace[i], 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));

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
	int ni = 0;
	int ti = 0;

	float scale = m_scale;

	// Top
	for (int i = 0; i < count; i++)
	{
		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, scale);
		newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, -1.0f);
	}

	// Bottom
	for (int i = 0; i < count; i++)
	{
		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, -scale);
		newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	// Sides
	for (int i = 0; i < count; i++)
	{
		int iNext = i == count - 1 ? 0 : i + 1;

		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, scale);
		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, -scale);
		newVertices[vi++].position = glm::vec3(m_polygonFace[iNext].x, m_polygonFace[iNext].y, -scale);
		newVertices[vi++].position = glm::vec3(m_polygonFace[iNext].x, m_polygonFace[iNext].y, scale);

		normal = glm::normalize(glm::cross(glm::vec3(m_polygonFace[iNext] - m_polygonFace[i], 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));

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
}
