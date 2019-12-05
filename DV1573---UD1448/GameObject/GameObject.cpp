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
	m_shouldRender = true;
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

	//Deletion of m_body is done in the destructor of BulletPhysics // nah bruh
	for (int i = 0; i < (int)m_bodies.size(); i++)
		removeBody(i);

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

				// Get skeleton
				Skeleton tempSkeleton = tempLoader.GetSkeleton(i);
				std::string skeletonName = tempSkeleton.name + "_" + m_objectName;
				if (skeletonName != "" && !SkeletonMap::getInstance()->existsWithName(skeletonName))
				{
					SkeletonMap::getInstance()->createSkeleton(skeletonName, tempSkeleton);
					logTrace("Skeleton created: {0}", skeletonName);
				}
				tempMesh.setSkeleton(skeletonName);
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

			
			tempMesh.setMaterial(tempLoader.GetMaterial(i).name);
			//Get the mesh pointer so that we don't have to always search through the MeshMap, when rendering
			tempMeshBox.mesh = MeshMap::getInstance()->createMesh(meshName, tempMesh); 
			logTrace("Mesh loaded: {0}, Expecting material: {1}", tempMesh.getName().c_str(), tempMesh.getMaterial());
		}
		else {
			tempMeshBox.mesh = MeshMap::getInstance()->getMesh(meshName);
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
				unsigned char* data = stbi_load(albedoFile.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D);

					tempMaterial.texture = true;
					tempMaterial.textureID.push_back(texture);
				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}
				stbi_image_free(data);
			}
			else
			{
				tempMaterial.texture = false;
			}

			if (tempLoader.GetNormalMap(i) != "-1")
			{
				std::string normalFile = TEXTUREPATH + tempLoader.GetNormalMap(i);
				GLuint normalMap;
				glGenTextures(1, &normalMap);
				glBindTexture(GL_TEXTURE_2D, normalMap);
				// set the texture wrapping/filtering options (on the currently bound texture object)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// load and generate the texture
				int width, height, nrChannels;
				unsigned char* data = stbi_load(normalFile.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D);

					tempMaterial.normalMap = true;
					tempMaterial.normalMapID.push_back(normalMap);
				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}
				stbi_image_free(data);
			}

			//Get the material pointer so that we don't have to always search through the MatMap, when rendering
			tempMeshBox.material = MaterialMap::getInstance()->createMaterial(materialName, tempMaterial);
 			logTrace("Material created: {0}", materialName);
		}
		else {
			tempMeshBox.material = MaterialMap::getInstance()->getMaterial(materialName);
		}

		m_meshes.push_back(tempMeshBox);						// This effectively adds the mesh to the gameobject
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

void GameObject::initMesh(Mesh mesh)
{
	MeshBox tempMeshBox;											// Meshbox holds the mesh identity and local transform to GameObject
	tempMeshBox.name = mesh.getName();
	m_meshes.push_back(tempMeshBox);								// This effectively adds the mesh to the gameobject
	if (!MeshMap::getInstance()->existsWithName(mesh.getName()))	// This creates the mesh if it does not exist (by name)
	{
		//Add mesh
		MeshMap::getInstance()->createMesh(mesh.getName(), mesh);
	}

	//Allocate all of the model matrixes
	m_modelMatrixes.resize(m_meshes.size());
	updateModelMatrix();
}

void GameObject::initMesh(std::string name, std::vector<Vertex> vertices, std::vector<Face> faces)
{
	MeshBox tempMeshBox;									// Meshbox holds the mesh identity and local transform to GameObject
	tempMeshBox.name = name;
							// This effectively adds the mesh to the gameobject
	if (!MeshMap::getInstance()->existsWithName(name))		// This creates the mesh if it does not exist (by name)
	{
		Mesh tempMesh;
		tempMesh.nameMesh(name);
		
		// Default mesh
		tempMesh.setUpMesh(vertices, faces);
		tempMesh.setUpBuffers();

		//Add mesh
		tempMeshBox.mesh = MeshMap::getInstance()->createMesh(name, tempMesh);

	}
	else {
		tempMeshBox.mesh = MeshMap::getInstance()->getMesh(name);
	}
	m_meshes.push_back(tempMeshBox);
	//Allocate all of the model matrixes
	m_modelMatrixes.resize(m_meshes.size());
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

//const GLuint& GameObject::getNormalMap() const
//{
//	return m_normalMap;
//}

//const GLuint& GameObject::getNormalMapTexture() const
//{
//	return m_normalMapTexture;
//}

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

void GameObject::setTransform(Transform transform, int meshIndex)
{
	m_meshes[meshIndex].transform = transform;
	updateModelMatrix();
}

void GameObject::setTransform(glm::vec3 worldPosition = glm::vec3(.0f), glm::quat worldRot = glm::quat(), glm::vec3 worldScale = glm::vec3(1.0f))
{
	m_transform.position = worldPosition;
	m_transform.scale = worldScale;
	m_transform.rotation = worldRot;
	updateModelMatrix();
}

void GameObject::setBtOffset(glm::vec3 offset, int meshIndex)
{
	m_meshes[meshIndex].btoffset = offset;
}

void GameObject::setWorldPosition(glm::vec3 worldPosition)
{
	m_lastPosition = m_transform.position;
	m_transform.position = worldPosition;
	updateModelMatrix();
}

void GameObject::setWorldPosition(glm::vec3 worldPosition, int meshIndex)
{
	m_meshes[meshIndex].transform.position = worldPosition;
	updateModelMatrix();
}

void GameObject::offsetMesh(glm::vec3 position, int meshIndex)
{
	Mesh* mesh = nullptr;
	if (m_meshes.size() > 0)
		mesh = MeshMap::getInstance()->getMesh(m_meshes[meshIndex].name);

	mesh->setPos(position);
	updateModelMatrix();
}

void GameObject::setBTWorldPosition(glm::vec3 worldPosition, int meshIndex)
{
	if (m_bodies[meshIndex])
	{
		btTransform newTransform = m_bodies[meshIndex]->getWorldTransform();
		newTransform.setOrigin(btVector3(worldPosition.x, worldPosition.y, worldPosition.z));
		m_bodies[meshIndex]->setWorldTransform(newTransform);
		updateBulletRigids();
		updateModelMatrix();
	}
}

void GameObject::setBTTransform(Transform transform, int meshIndex)
{
	btTransform newTransform = m_bodies[meshIndex]->getWorldTransform();
	newTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
	newTransform.setRotation(btQuaternion(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w));
	m_bodies[meshIndex]->setWorldTransform(newTransform);
	updateBulletRigids();
	updateModelMatrix();
}

void GameObject::set_BtActive(bool state, int meshIndex)
{
	m_bodies[meshIndex]->setActivationState(state);
}

void GameObject::removeBody(int bodyIndex)
{
	if (m_bodies[bodyIndex])
	{
		m_bPhysics->removeObject(m_bodies[bodyIndex]);
		m_bodies[bodyIndex] = nullptr;
	}
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

void GameObject::setMaterial(std::string matName, int meshIndex)
{
	if (meshIndex == -1)
	{
		for (int i = 0; i < (int)m_meshes.size(); i++)
		{
			Mesh* mesh = MeshMap::getInstance()->getMesh(m_meshes[i].name);
			if (mesh)
				mesh->setMaterial(matName);
		}
	}
	else if (meshIndex == -2)
	{
		Mesh* mesh = MeshMap::getInstance()->getMesh(m_meshes[0].name);
		if (mesh)
		{
			std::string mat = mesh->getMaterial();
			for (int i = 1; i < (int)m_meshes.size(); i++)
			{
				Mesh* mesh = MeshMap::getInstance()->getMesh(m_meshes[i].name);
				mesh->setMaterial(mat);
			}
		}
	}
	else
	{
		Mesh* mesh = MeshMap::getInstance()->getMesh(m_meshes[meshIndex].name);
		if (mesh)
			mesh->setMaterial(matName);
	}

}

const Transform GameObject::getTransform() const
{
	//Mesh* mesh = nullptr;
	//if (m_meshes.size() > 0)
	//	mesh = MeshMap::getInstance()->getMesh(m_meshes[0].name);
	
	// Adds the inherited transforms together to get the world position of a mesh
	Transform world_transform;
	if (m_meshes.size() > 0)
	{
		world_transform.position = m_transform.position + m_meshes[0].transform.position;
		world_transform.rotation = m_transform.rotation * m_meshes[0].transform.rotation;
		world_transform.scale = m_transform.scale * m_meshes[0].transform.scale;
	}
	else
	{
		world_transform.position = m_transform.position;
		world_transform.rotation = m_transform.rotation;
		world_transform.scale = m_transform.scale;
	}

	return world_transform;
}

Material* GameObject::getMaterial(const int& meshIndex)
{
	return m_meshes[meshIndex].material;
}

Mesh* GameObject::getMesh(const int& meshIndex)
{
	return m_meshes[meshIndex].mesh;
}

const Transform GameObject::getTransform(int meshIndex) const
{
	//Mesh* mesh = MeshMap::getInstance()->getMesh(m_meshes[meshIndex].name); //This costs a lot //True we get rid off

	// Adds the inherited transforms together to get the world position of a mesh
	Transform world_transform;
	world_transform.position = m_transform.position + m_meshes[meshIndex].transform.position;
	world_transform.rotation = m_transform.rotation * m_meshes[meshIndex].transform.rotation;
	world_transform.scale = m_transform.scale * m_meshes[meshIndex].transform.scale;

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

const Transform GameObject::getTransformMesh(int meshIndex) const
{
	Mesh* mesh = MeshMap::getInstance()->getMesh(m_meshes[meshIndex].name);

	// Adds the inherited transforms together to get the world position of a mesh
	Transform world_transform;
	world_transform.position = m_meshes[meshIndex].transform.position + mesh->getTransform().position;
	world_transform.rotation = m_meshes[meshIndex].transform.rotation * mesh->getTransform().rotation;
	world_transform.scale = m_meshes[meshIndex].transform.scale * mesh->getTransform().scale;

	return world_transform;
}

const Transform GameObject::getTransformRigid(int meshIndex) const
{
	if (!m_bodies[meshIndex])
		Transform newTransform;

	btVector3 rigidBodyPos = m_bodies[meshIndex]->getWorldTransform().getOrigin();

	btTransform rigidBodyTransform = m_bodies[meshIndex]->getWorldTransform();
	Transform newTransform;
	newTransform.position.x = rigidBodyTransform.getOrigin().getX();
	newTransform.position.y = rigidBodyTransform.getOrigin().getY();
	newTransform.position.z = rigidBodyTransform.getOrigin().getZ();

	newTransform.rotation.x = rigidBodyTransform.getRotation().getX();
	newTransform.rotation.y = rigidBodyTransform.getRotation().getY();
	newTransform.rotation.z = rigidBodyTransform.getRotation().getZ();
	newTransform.rotation.w = rigidBodyTransform.getRotation().getW();

	newTransform.scale = getTransformMesh(meshIndex).scale;

	return newTransform;
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
	//logWarning("Material: {0}", materialName);
	shader->setMaterial(materialName);
}


void GameObject::bindMaterialToShader(Shader* shader, Material* material)
{
	shader->setMaterial(material);
}
void GameObject::unbindMaterialFromShader(Shader* shader, const std::string& materialName)
{
	shader->unbindMaterial(materialName);
}
void GameObject::unbindMaterialFromShader(Shader* shader, Material* material)
{
	shader->unbindMaterial(material);
}
void GameObject::createRigidBody(CollisionObject shape, BulletPhysics* bp)
{
	if (!m_bPhysics)
		m_bPhysics = bp;

	if (m_bPhysics == nullptr && bp == nullptr)
		return;

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

			m_bodies.emplace_back(m_bPhysics->createObject(shape, 0.0f, center, halfSize));
			m_bodies.back()->setUserPointer(this);
			//setTransformFromRigid(i);
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

			m_bodies.emplace_back(m_bPhysics->createObject(shape, 0.0f, center, halfSize, getTransform(i).rotation));
			m_bodies.back()->setUserPointer(this);
			//setTransformFromRigid(i);

		}
	}

	m_transform.position = glm::vec3(0.0f);
	m_transform.rotation = glm::quat();
}

void GameObject::createDynamicRigidBody(CollisionObject shape, BulletPhysics* bp, float weight)
{
	if (!m_bPhysics)
		m_bPhysics = bp;

	m_bodies.clear();
	m_bodies.shrink_to_fit();
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		const std::vector<Vertex>& vertices = MeshMap::getInstance()->getMesh(m_meshes[i].name)->getVertices();

		
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

		m_bodies.emplace_back(m_bPhysics->createObject(shape, weight, center, halfSize, getTransform(i).rotation, true, 0.1f, 8.0f));
		m_bodies.back()->setUserPointer(this);
		m_bodies.back()->setGravity(btVector3(0.0f, -25.0f, 0.0f));
		setTransformFromRigid(i);
	}

	m_transform.position = glm::vec3(0.0f);
	m_transform.rotation = glm::quat();
}

void GameObject::createDynamicRigidBody(CollisionObject shape, BulletPhysics* bp, float weight, int meshIndex, bool recenter)
{
	if (!m_bPhysics)
		m_bPhysics = bp;

	const std::vector<Vertex>& vertices = MeshMap::getInstance()->getMesh(m_meshes[meshIndex].name)->getVertices();

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

	glm::vec3 center = getTransform(meshIndex).position;
	if (!recenter)
		glm::vec3 center = glm::vec3((min + max) * 0.5f) + getTransform(meshIndex).position;

	glm::vec3 halfSize = glm::vec3((max - min) * 0.5f) * getTransform(meshIndex).scale;

	m_bodies.emplace_back(m_bPhysics->createObject(shape, weight, center, halfSize, getTransform(meshIndex).rotation, true, 0.0f, 1.0f));

	m_bodies.back()->setUserPointer(this);
	m_bodies.back()->setGravity(btVector3(0.0f, -25.0f, 0.0f));

	m_transform.position = glm::vec3(0.0f);
	m_transform.rotation = glm::quat();

	setTransformFromRigid(meshIndex);
}

void GameObject::updateBulletRigids()
{
	for (int i = 0; i < (int)m_bodies.size(); i++)
	{
		setTransformFromRigid(i);
	}
}

void GameObject::setTransformFromRigid(int i)
{
	if (!m_bodies[i])
		return;

	btVector3& rigidBodyPos = m_bodies[i]->getWorldTransform().getOrigin();

	const btTransform& rigidBodyTransform = m_bodies[i]->getWorldTransform();
	const btVector3& btOrigin = rigidBodyTransform.getOrigin();
	t_transform.position.x = btOrigin.getX();
	t_transform.position.y = btOrigin.getY();
	t_transform.position.z = btOrigin.getZ();

	const btQuaternion& btRotation = rigidBodyTransform.getRotation();
	t_transform.rotation.x = btRotation.getX();
	t_transform.rotation.y = btRotation.getY();
	t_transform.rotation.z = btRotation.getZ();
	t_transform.rotation.w = btRotation.getW();

	t_transform.scale = getTransformMesh(i).scale;

	setTransform(t_transform, i);
}

//void GameObject::loadNormalMap()
//{
//}
