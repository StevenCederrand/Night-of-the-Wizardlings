#include <Pch/Pch.h>
#include <Loader/BGLoader.h>
#include "GameObject.h"

GameObject::GameObject()
{
	m_objectName = "Empty";
	m_type = 0;
	m_bPhysics = nullptr;
	m_shouldRender = true;
}

GameObject::GameObject(std::string objectName)
{
	m_objectName = objectName;
	m_type = 0;
	m_bPhysics = nullptr;
}

GameObject::~GameObject()
{
	//TODO: fix deletion of textures
	for (int i = 0; i < (int)m_meshes.size(); i++)
	{
		// We create the textures in this class so we delete them here for consistency
		//Material* material = MaterialMap::getInstance()->getMaterial(MeshMap::getInstance()->getMesh(m_meshes[i].name)->getMaterial());
		//if (material)
		//	for (int j = 0; j < (int)material->textureID.size(); j++)
		//		glDeleteTextures(1, &material->textureID[j]);
	}

	for (DebugDrawer* dd : m_debugDrawers)
		if(dd != nullptr)
			delete dd;

	//Deletion of m_body is done in the destructor of BulletPhysics
}

void GameObject::loadMesh(std::string fileName)
{
	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + fileName);

	for (int i = 0; i < tempLoader.GetMeshCount(); i++)
	{
		// Get mesh
		MeshBox tempMeshBox;									// Meshbox holds the mesh identity and local transform to GameObject
		std::string meshName = tempLoader.GetMeshName(i);
		tempMeshBox.name = meshName;
		tempMeshBox.transform = tempLoader.GetTransform(i);		// One way of getting the meshes transform
		m_meshes.push_back(tempMeshBox);						// This effectively adds the mesh to the gameobject
		if (!MeshMap::getInstance()->existsWithName(meshName))	// This creates the mesh if it does not exist (by name)
		{
			Mesh tempMesh;
			tempMesh.saveFilePath(tempLoader.GetFileName(), i);
			tempMesh.nameMesh(tempLoader.GetMeshName(i));
			if (tempLoader.GetSkeleton(i).name != "")
			{
				// Mesh with skeleton requires extra vertex data
				tempMesh.setUpMesh(tempLoader.GetSkeleVertices(i), tempLoader.GetFaces(i));
				tempMesh.setUpSkeleBuffers();
			}
			else
			{
				// Default mesh
				tempMesh.setUpMesh(tempLoader.GetVertices(i), tempLoader.GetFaces(i));
				tempMesh.setUpBuffers();
			}

			// other way of getting the meshes transform
			// Value that may or may not be needed depening on how we want the meshes default position to be
			// Needs more testing, this value is per global mesh, the MeshBox value is per GameObject mesh
			// tempMesh.setTransform(tempLoader.GetTransform(id));

			// Get skeleton
			Skeleton tempSkeleton = tempLoader.GetSkeleton(i);
			std::string skeletonName = tempSkeleton.name + "_" + m_objectName;
			if (skeletonName != "" && !SkeletonMap::getInstance()->existsWithName(skeletonName))
			{
				SkeletonMap::getInstance()->createSkeleton(skeletonName, tempSkeleton);
				logTrace("Skeleton created: {0}", skeletonName);
			}

			// Get animation
			for (size_t a = 0; a < tempLoader.GetAnimation(i).size(); a++)
			{
				Animation tempAnimation = tempLoader.GetAnimation(i)[a];
				std::string animationName = tempAnimation.name + "_" + m_objectName;
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
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

				tempMaterial.texture = true;
				tempMaterial.textureID.push_back(texture);
			}
			else
			{
				tempMaterial.texture = false;
			}

			MaterialMap::getInstance()->createMaterial(materialName, tempMaterial);
			logTrace("Material created: {0}", materialName);
		}
	}

	//Allocate all of the model matrixes
	m_modelMatrixes.resize(m_meshes.size());
	for (size_t i = 0; i < m_modelMatrixes.size(); i++)
	{
		m_modelMatrixes[i] = glm::mat4(1.0);
	}

	tempLoader.Unload();
	updateModelMatrix();
}

const bool& GameObject::getShouldRender() const
{
	return m_shouldRender;
}

const glm::vec3 GameObject::getLastPosition() const
{
	return m_lastPosition;
}

//Update each individual modelmatrix for the meshes
void GameObject::updateModelMatrix() {
	
	Transform transform;
	for (size_t i = 0; i < m_modelMatrixes.size(); i++)
	{
		m_modelMatrixes[i] = glm::mat4(1.0f);
		transform = getTransform(i);

		m_modelMatrixes[i] = glm::translate(m_modelMatrixes.at(i), transform.position);
		m_modelMatrixes[i] *= glm::mat4_cast(transform.rotation);
		m_modelMatrixes[i] = glm::scale(m_modelMatrixes[i], transform.scale);
	}
}

void GameObject::setTransform(Transform transform)
{
	m_transform = transform;
	updateModelMatrix();
}

void GameObject::setTransform(glm::vec3 worldPosition = glm::vec3(.0f), glm::quat worldRot = glm::quat(), glm::vec3 worldScale = glm::vec3(1.0f))
{
	m_transform.position = worldPosition;
	m_transform.scale = worldScale;
	m_transform.rotation = worldRot;
	updateModelMatrix();
}

void GameObject::setWorldPosition(glm::vec3 worldPosition)
{
	m_lastPosition = m_transform.position;
	m_transform.position = worldPosition;
	updateModelMatrix();
}

void GameObject::translate(const glm::vec3& translationVector)
{
	m_transform.position += translationVector;
	updateModelMatrix();
}

void GameObject::setShouldRender(bool condition)
{
	m_shouldRender = condition;
}

void GameObject::setRestitution(float restitution)
{
	m_restitution = restitution;
}

const Transform GameObject::getTransform() const
{
	Mesh* mesh = nullptr;
	if (m_meshes.size() > 0)
		mesh = MeshMap::getInstance()->getMesh(m_meshes[0].name);
	
	// Adds the inherited transforms together to get the world position of a mesh
	Transform world_transform;
	if (mesh)
	{
		world_transform.position = m_transform.position + m_meshes[0].transform.position + mesh->getTransform().position;
		world_transform.rotation = m_transform.rotation * m_meshes[0].transform.rotation *  mesh->getTransform().rotation;
		world_transform.scale = m_transform.scale * m_meshes[0].transform.scale * mesh->getTransform().scale;
	}
	else
	{
		world_transform.position = m_transform.position;
		world_transform.rotation = m_transform.rotation;
		world_transform.scale = m_transform.scale;
	}

	return world_transform;
}

const Transform GameObject::getTransform(int meshIndex) const
{
	Mesh* mesh = MeshMap::getInstance()->getMesh(m_meshes[meshIndex].name); //This costs a lot

	// Adds the inherited transforms together to get the world position of a mesh
	Transform world_transform;
	world_transform.position = m_transform.position + m_meshes[meshIndex].transform.position + mesh->getTransform().position;
	world_transform.rotation = m_transform.rotation * m_meshes[meshIndex].transform.rotation * mesh->getTransform().rotation;
	world_transform.scale = m_transform.scale * m_meshes[meshIndex].transform.scale * mesh->getTransform().scale;

	return world_transform;
}

const Transform& GameObject::getTransform(Mesh* mesh, const int& meshIndex) const
{
	// Adds the inherited transforms together to get the world position of a mesh
	Transform world_transform;
	world_transform.position = m_transform.position + m_meshes[meshIndex].transform.position + mesh->getTransform().position;
	world_transform.rotation = m_transform.rotation * m_meshes[meshIndex].transform.rotation * mesh->getTransform().rotation;
	world_transform.scale = m_transform.scale * m_meshes[meshIndex].transform.scale * mesh->getTransform().scale;

	return world_transform;
}

const std::string& GameObject::getMeshName(int meshIndex) const
{
	return m_meshes[meshIndex].name;
}

const glm::mat4& GameObject::getMatrix(const int& i) const
{
	if (m_modelMatrixes.size() == 0) {
		return glm::mat4(1.0f);
	}
	//if we are trying to access a matrix beyond our count
	if (i > static_cast<int>(m_modelMatrixes.size())) {
		return glm::mat4(1.0f);
	}
	return m_modelMatrixes[i];
}

void GameObject::bindMaterialToShader(std::string shaderName)
{
	ShaderMap::getInstance()->getShader(shaderName)->setMaterial(MeshMap::getInstance()->getMesh(m_meshes[0].name)->getMaterial());
}

void GameObject::bindMaterialToShader(std::string shaderName, int meshIndex)
{
	ShaderMap::getInstance()->getShader(shaderName)->setMaterial(MeshMap::getInstance()->getMesh(m_meshes[meshIndex].name)->getMaterial());
}

void GameObject::bindMaterialToShader(Shader* shader, const int& meshIndex)
{
	shader->setMaterial(MeshMap::getInstance()->getMesh(m_meshes[meshIndex].name)->getMaterial());
}

void GameObject::bindMaterialToShader(Shader* shader, const std::string& materialName)
{
	shader->setMaterial(materialName);
}
void GameObject::bindMaterialToShader(Shader* shader, Material* material)
{
	shader->setMaterial(material);
}
void GameObject::createRigidBody(CollisionObject shape, BulletPhysics* bp)
{
	if (!m_bPhysics)
		m_bPhysics = bp;

	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		
		const std::vector<Vertex>& vertices = MeshMap::getInstance()->getMesh(m_meshes[i].name)->getVertices();

		// Animated mesh case
		if (vertices.size() == 0)
		{
			const std::vector<Vertex2>& vertices2 = MeshMap::getInstance()->getMesh(m_meshes[i].name)->getVerticesSkele();

			glm::vec3 min = vertices2[0].position;
			glm::vec3 max = vertices2[0].position;

			for (size_t i = 1; i < vertices2.size(); i++)
			{
				min.x = fminf(vertices2[i].position.x, min.x);
				min.y = fminf(vertices2[i].position.y, min.y);
				min.z = fminf(vertices2[i].position.z, min.z);

				max.x = fmaxf(vertices2[i].position.x, max.x);
				max.y = fmaxf(vertices2[i].position.y, max.y);
				max.z = fmaxf(vertices2[i].position.z, max.z);
			}

			glm::vec3 center = glm::vec3((min + max) * 0.5f) + getTransform(i).position;
			glm::vec3 halfSize = glm::vec3((max - min) * 0.5f) * getTransform(i).scale;
			// TODO: ROTATE
			m_bodies.emplace_back(m_bPhysics->createObject(shape, 0.0f, center, halfSize));
		}
		else
		{
			glm::vec3 min = vertices[0].position;
			glm::vec3 max = vertices[0].position;

			for (size_t i = 1; i < vertices.size(); i++)
			{
				min.x = fminf(vertices[i].position.x, min.x);
				min.y = fminf(vertices[i].position.y, min.y);
				min.z = fminf(vertices[i].position.z, min.z);

				max.x = fmaxf(vertices[i].position.x, max.x);
				max.y = fmaxf(vertices[i].position.y, max.y);
				max.z = fmaxf(vertices[i].position.z, max.z);
			}

			glm::vec3 center = glm::vec3((min + max) * 0.5f) + getTransform(i).position;
			glm::vec3 halfSize = glm::vec3((max - min) * 0.5f) * getTransform(i).scale;
			// TODO: ROTATE

			m_bodies.emplace_back(m_bPhysics->createObject(shape, 0.0f, center, halfSize, getTransform(i).rotation, m_restitution));
		}
	}
}

void GameObject::createDebugDrawer()
{
	for (size_t i = 0; i < m_bodies.size(); i++)
	{
		// Temporarily off, rotation of drawers do not work
		//m_debugDrawers.emplace_back(new DebugDrawer());
		//m_debugDrawers[i]->setUpMesh(*m_bodies[i]);
	}
}
