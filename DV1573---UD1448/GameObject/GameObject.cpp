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

	//GLuint texture = mat->textureID[0];
	glGenTextures(1, &tempMaterial.textureID[0]);
	//////VAFAN GÖR VI HÄR? NICO
	glBindTexture(GL_TEXTURE_2D, tempMaterial.textureID[0]);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(m_materialName.c_str(), &width, &height, &nrChannels, 0);
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