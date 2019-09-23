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

	//Get the mesh Materials
	for (int i = 0; i < tempLoader.GetMaterialCount(); i++)
	{
		Material tempMaterial = tempLoader.GetMaterial();
		std::string materialName = (std::string)tempLoader.GetMaterial().name;
		MaterialMap::getInstance()->createMaterial(materialName, tempMaterial);

		m_materialNames.push_back(materialName);
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
