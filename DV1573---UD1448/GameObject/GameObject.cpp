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

	//Get file mesh models
	for (int i = 0; i < tempLoader.GetMeshCount(); i++)
	{
		m_meshes.push_back(new Mesh());
		int id = (int)m_meshes.size() - 1;
		m_meshes[id]->saveFilePath(tempLoader.GetFileName(), i);
		m_meshes[id]->nameMesh(tempLoader.GetMeshName(i));
		m_meshes[id]->setUpMesh(tempLoader.GetVertices(i), tempLoader.GetFaces(i));
		m_meshes[id]->setUpBuffers();
		m_meshes[id]->setPos(tempLoader.GetPosition(id));
		m_meshes[id]->setRot(tempLoader.GetRotation(id));
		m_meshes[id]->setScale(tempLoader.GetScale(id));
		m_meshes[id]->setMaterial(tempLoader.GetMaterial(id).name);
		
		logTrace("Mesh loaded: {0}, Expecting material: {1}", m_meshes[id]->getName().c_str(), m_meshes[id]->getMaterial());
		Material tempMaterial = tempLoader.GetMaterial(id);
		std::string materialName = tempMaterial.name;

		// Get material
		if (!MaterialMap::getInstance()->existsWithName(materialName))
		{
			if (tempLoader.GetAlbedo() != "-1")
			{
				std::string albedoFile = TEXTUREPATH + tempLoader.GetAlbedo(id);
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
			logTrace("Material created: {0}", materialName);
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
	m_transform.position = worldPosition;
	m_transform.rotation = worldRot;
	m_transform.scale = worldScale;
}

void GameObject::setWorldPosition(glm::vec3 worldPosition)
{
	m_transform.position = worldPosition;
}

void GameObject::translate(const glm::vec3& translationVector)
{
	m_transform.position += translationVector;
}

const Transform GameObject::getTransform() const
{
	Transform world_transform;
	world_transform.position = m_transform.position + m_meshes[0]->getTransform().position;
	world_transform.rotation = m_transform.rotation + m_meshes[0]->getTransform().rotation;
	world_transform.scale = m_transform.scale * m_meshes[0]->getTransform().scale;

	return world_transform;
}

const Transform GameObject::getTransform(int meshIndex) const
{
	Transform world_transform;
	world_transform.position = m_transform.position + m_meshes[meshIndex]->getTransform().position;
	world_transform.rotation = m_transform.rotation + m_meshes[meshIndex]->getTransform().rotation;
	world_transform.scale = m_transform.scale * m_meshes[meshIndex]->getTransform().scale;

	return world_transform;
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
	ShaderMap::getInstance()->getShader(shaderName)->setMaterial(m_meshes[0]->getMaterial());
}

void GameObject::bindMaterialToShader(std::string shaderName, int meshIndex)
{
	ShaderMap::getInstance()->getShader(shaderName)->setMaterial(m_meshes[meshIndex]->getMaterial());
}
