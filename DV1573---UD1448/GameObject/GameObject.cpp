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
	
}

void GameObject::loadMesh(std::string fileName)
{
	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + fileName);

	for (int i = 0; i < tempLoader.GetMeshCount(); i++)
	{
		// Get mesh
		MeshBox tempMeshBox;								// Meshbox holds the mesh identity and local transform to GameObject
		std::string meshName = tempLoader.GetMeshName(i);
		tempMeshBox.name = meshName;
		tempMeshBox.transform = tempLoader.GetTransform(i);	// One way of getting the meshes transform
		m_meshes.push_back(tempMeshBox);					// This effectively adds the mesh to the gameobject
		if (!MeshMap::getInstance()->existsWithName(meshName))	// This creates the mesh if it does not exist (by name)
		{
			Mesh tempMesh;
			tempMesh.saveFilePath(tempLoader.GetFileName(), i);
			tempMesh.nameMesh(tempLoader.GetMeshName(i));
			tempMesh.setUpMesh(tempLoader.GetVertices(i), tempLoader.GetFaces(i));
			tempMesh.setUpBuffers();

			// other way of getting the meshes transform
			// Value that may or may not be needed depening on how we want the meshes default position to be
			// Needs more testing, this value is per global mesh", the MeshBox value is per GameObject mesh
			//tempMesh.setTransform(tempLoader.GetTransform(id));

			// Get skeleton
			Skeleton tempSkeleton = tempLoader.GetSkeleton(i);
			std::string skeletonName = tempSkeleton.name;
			if (skeletonName != "" && !SkeletonMap::getInstance()->existsWithName(skeletonName))
			{
				SkeletonMap::getInstance()->createSkeleton(skeletonName, tempSkeleton);
				logTrace("Skeleton created: {0}", skeletonName);
			}

			// Get animation
			for (int a = 0; a < tempLoader.GetAnimation(i).size(); a++)
			{
				Animation tempAnimation = tempLoader.GetAnimation(i)[a];
				std::string animationName = tempAnimation.name;
				if (animationName != "" && !AnimationMap::getInstance()->existsWithName(animationName))
				{
					AnimationMap::getInstance()->createAnimation(animationName, tempAnimation);
					logTrace("Animation created: {0}", animationName);
				}

				tempMesh.addAnimation(animationName);
			}

			tempMesh.setSkeleton(skeletonName);
			tempMesh.setMaterial(tempLoader.GetMaterial(i).name);
			MeshMap::getInstance()->createMesh(meshName, tempMesh);
			logTrace("Mesh loaded: {0}, Expecting material: {1}", tempMesh.getName().c_str(), tempMesh.getMaterial());
		}

		// Get material
		Material tempMaterial = tempLoader.GetMaterial(i);
		std::string materialName = tempMaterial.name;
		if (!MaterialMap::getInstance()->existsWithName(materialName)) 	// This creates the material if it does not exist (by name)
		{
			if (tempLoader.GetAlbedo(i) != "-1")
			{
				std::string albedoFile = TEXTUREPATH + tempLoader.GetAlbedo(i);
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

void GameObject::setTransform(glm::vec3 worldPosition = glm::vec3(.0f), glm::quat worldRot = glm::quat(), glm::vec3 worldScale = glm::vec3(1.0f))
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
	Mesh* mesh = MeshMap::getInstance()->getMesh(m_meshes[0].name);
	
	// Adds the inherited transforms together to get the world position of a mesh
	Transform world_transform;
	world_transform.position = m_transform.position + m_meshes[0].transform.position + mesh->getTransform().position;
	world_transform.rotation = m_transform.rotation + m_meshes[0].transform.rotation +  mesh->getTransform().rotation;
	world_transform.scale = m_transform.scale * m_meshes[0].transform.scale * mesh->getTransform().scale;

	return world_transform;
}

const Transform GameObject::getTransform(int meshIndex) const
{
	Mesh* mesh = MeshMap::getInstance()->getMesh(m_meshes[meshIndex].name);

	// Adds the inherited transforms together to get the world position of a mesh
	Transform world_transform;
	world_transform.position = m_transform.position + m_meshes[meshIndex].transform.position + mesh->getTransform().position;
	world_transform.rotation = m_transform.rotation + m_meshes[meshIndex].transform.rotation + mesh->getTransform().rotation;
	world_transform.scale = m_transform.scale * m_meshes[meshIndex].transform.scale * mesh->getTransform().scale;

	return world_transform;
}

Mesh* GameObject::getMesh() const
{
	//TODO: Consider removing function
	return MeshMap::getInstance()->getMesh(m_meshes[0].name);
}

Mesh* GameObject::getMesh(int index) const
{
	//TODO: Consider removing function
	return MeshMap::getInstance()->getMesh(m_meshes[index].name);
}

const std::string& GameObject::getMeshN(int meshIndex) const
{
	return m_meshes[meshIndex].name;
}

void GameObject::bindMaterialToShader(std::string shaderName)
{
	ShaderMap::getInstance()->getShader(shaderName)->setMaterial(MeshMap::getInstance()->getMesh(m_meshes[0].name)->getMaterial());
}

void GameObject::bindMaterialToShader(std::string shaderName, int meshIndex)
{
	
	ShaderMap::getInstance()->getShader(shaderName)->setMaterial(MeshMap::getInstance()->getMesh(m_meshes[meshIndex].name)->getMaterial());
}
