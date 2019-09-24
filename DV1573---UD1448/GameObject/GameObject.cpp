#include <Pch/Pch.h>
#include <Loader/BGLoader.h>
#include "GameObject.h"

GameObject::GameObject()
{
	m_objectName = "Empty";

}

GameObject::GameObject(std::string objectName)
{
	m_objectName = objectName;
}

GameObject::~GameObject()
{
	for (Mesh* mesh : m_meshes)
		delete mesh;

}

void GameObject::loadMesh(std::string fileName)
{
	BGLoader tempLoader;
	tempLoader.LoadMesh(MESHPATH + fileName);

	//Get mesh model
	m_meshes.push_back(new Mesh());
	m_meshes[m_meshes.size() - 1]->saveFilePath(tempLoader.GetFileName(), 0);
	m_meshes[m_meshes.size() - 1]->nameMesh(tempLoader.GetMeshName());
	m_meshes[m_meshes.size() - 1]->setUpMesh(tempLoader.GetVertices(), tempLoader.GetFaces());
	m_meshes[m_meshes.size() - 1]->setUpBuffers();
	logTrace("Mesh loaded: {0}", m_meshes[m_meshes.size() - 1]->getName().c_str());

	//Get the mesh Materials
	for (int i = 0; i < tempLoader.GetMaterialCount(); i++)
	{
		Material tempMaterial = tempLoader.GetMaterial();
		std::string materialName = tempMaterial.name;
		m_materialNames.push_back(materialName);

		if (!MaterialMap::getInstance()->existsWithName(materialName))
		{
			if (tempLoader.GetAlbedo() != "-1")
			{
				std::string albedoFile = TEXTUREPATH + tempLoader.GetAlbedo();
				GLuint texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				// set the texture wrapping/filtering options (on the currently bound texture object)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// load and generate the texture
				int width, height, nrChannels;
				unsigned char* data = stbi_load(albedoFile.c_str(), &width, &height, &nrChannels, NULL);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D);
				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}
				stbi_image_free(data);
				tempMaterial.textureID.push_back(texture);
			}
			MaterialMap::getInstance()->createMaterial(materialName, tempMaterial);
			logTrace("Material created and used: {0}", materialName);
		}
		else
		{
			logTrace("Using existing material: {0}", materialName);
		}
	}


	tempLoader.Unload();
}

void GameObject::setTransform(Transform transform)
{
	m_transform = transform;
}

void GameObject::setTransform(glm::vec3 worldPosition = glm::vec3(.0f), glm::quat worldRot = glm::quat(), glm::vec3 worldScale = glm::vec3(.0f))
{
	m_transform.m_worldPos = worldPosition;
	m_transform.m_worldRot = worldRot;
	m_transform.m_worldScale = worldScale;
}

void GameObject::setWorldPosition(glm::vec3 worldPosition)
{
	m_transform.m_worldPos = worldPosition;
}

void GameObject::translate(const glm::vec3& translationVector)
{
	m_transform.m_worldPos += translationVector;
}

const Transform& GameObject::getTransform() const
{
	return m_transform;
}

Mesh* GameObject::getMesh() const
{
	return m_meshes[0];
}

Mesh* GameObject::getMesh(int index) const
{
	return m_meshes[index];
}

const std::vector<Mesh*>& GameObject::getMeshes() const
{
	return m_meshes;
}

void GameObject::bindMaterialToShader(std::string shaderName)
{
	ShaderMap::getInstance()->getShader(shaderName)->setMaterial(m_materialNames[0]);
}

void GameObject::bindMaterialToShader(std::string shaderName, int matIndex)
{
	if (matIndex > m_materialNames.size())
		logError("Binding material outside of range");
	ShaderMap::getInstance()->getShader(shaderName)->setMaterial(m_materialNames[matIndex]);
}
