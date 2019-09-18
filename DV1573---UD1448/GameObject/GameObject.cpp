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
	if (m_mesh != nullptr) {
		delete m_mesh;
	}
}

void GameObject::loadMesh(std::string meshName)
{
	BGLoader tempLoader;
	//----BUG----
	//A memory leak builds here
	tempLoader.LoadMesh(MESHPATH + meshName);
	
	
	m_mesh = new Mesh();
   	m_mesh->setUpMesh(tempLoader.GetVertices(0),
		tempLoader.GetVertexCount(0),
		tempLoader.GetFaces(0),
		tempLoader.GetFaceCount(0));
	m_mesh->setUpBuffers();

	//Get the mesh Material
	Material tempMaterial = tempLoader.GetMaterial(0);
	m_materialName = (std::string)tempLoader.GetMaterial(0).name;
	MaterialMap::getInstance()->createMaterial(m_materialName, tempMaterial);
	//----BUG----And is never unloaded
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
	return m_mesh;
}

void GameObject::bindMaterialToShader(std::string shaderName)
{
	ShaderMap::getInstance()->getShader(shaderName)->setMaterial(m_materialName);
}