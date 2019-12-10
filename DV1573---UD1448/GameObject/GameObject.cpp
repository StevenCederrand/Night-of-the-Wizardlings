#include <Pch/Pch.h>
#include <Loader/BGLoader.h>
#include "GameObject.h"

GameObject::GameObject()
{
	m_objectName = "Empty";
	m_type = 0;
	m_shouldRender = true;

	removeParticle = false;
}

GameObject::GameObject(std::string objectName)
{
	m_objectName = objectName;
	m_type = 0;
	m_shouldRender = true;

	removeParticle = false;
}

GameObject::~GameObject()
{
	for (int i = 0; i < (int)m_meshes.size(); i++)
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
			tempLoader.GetLoaderVertices(0)[0].tangent;

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
	updateTransform();
}

void GameObject::initMesh(Mesh mesh)
{
	MeshBox tempMeshBox;											// Meshbox holds the mesh identity and local transform to GameObject
	m_meshes.push_back(tempMeshBox);								// This effectively adds the mesh to the gameobject
	if (!MeshMap::getInstance()->existsWithName(mesh.getName()))	// This creates the mesh if it does not exist (by name)
	{
		//Add mesh
		MeshMap::getInstance()->createMesh(mesh.getName(), mesh);
	}

	//Allocate all of the model matrixes
	m_modelMatrixes.resize(m_meshes.size());
	updateTransform();
}

void GameObject::initMesh(std::string name, std::vector<Vertex> vertices, std::vector<Face> faces)
{
	MeshBox tempMeshBox;									// Meshbox holds the mesh identity and local transform to GameObject
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
	updateTransform();
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
void GameObject::updateTransform() {
	
	Transform transform;
	for (size_t i = 0; i < m_modelMatrixes.size(); i++)
	{
		m_modelMatrixes[i] = glm::mat4(1.0f);
		transform = getTransform(i);

		m_modelMatrixes[i] = glm::translate(m_modelMatrixes.at(i), transform.position);
		m_modelMatrixes[i] *= glm::mat4_cast(transform.rotation);
		m_modelMatrixes[i] = glm::scale(m_modelMatrixes[i], transform.scale);
	}

	m_lastPosition = m_transform.position;
}

void GameObject::setTransform(Transform transform)
{
	m_transform = transform;
	updateTransform();
}

void GameObject::setTransform(glm::vec3 worldPosition, glm::quat worldRot, glm::vec3 worldScale)
{
	m_transform.position = worldPosition;
	m_transform.scale = worldScale;
	m_transform.rotation = worldRot;
	updateTransform();
}

void GameObject::setMeshOffsetTransform(Transform transform, int meshIndex)
{
	m_meshes[meshIndex].transform = transform;
	updateTransform();
}

void GameObject::setWorldPosition(glm::vec3 worldPosition)
{
	m_transform.position = worldPosition;
	updateTransform();
}

void GameObject::setWorldRotation(glm::quat worldRotation)
{
	m_transform.rotation = worldRotation;
	updateTransform();
}

void GameObject::setMeshOffsetPosition(glm::vec3 position, int meshIndex)
{
	m_meshes[meshIndex].transform.position = position;
	updateTransform();
}

void GameObject::setMeshOffsetRotation(glm::quat rotation, int meshIndex)
{
	m_meshes[meshIndex].transform.rotation = rotation;
	updateTransform();
}

void GameObject::setBodyWorldPosition(glm::vec3 worldPosition, int meshIndex)
{
	if (m_meshes[meshIndex].body)
	{
		btTransform newTransform = m_meshes[meshIndex].body->getWorldTransform();
		newTransform.setOrigin(btVector3(worldPosition.x, worldPosition.y, worldPosition.z));
		m_meshes[meshIndex].body->setWorldTransform(newTransform);

		updateBulletRigids();
		updateTransform();
	}
}

void GameObject::setBodyActive(bool state, int meshIndex)
{
	if (m_meshes[meshIndex].body)
	{
		m_meshes[meshIndex].body->setActivationState(state);
	}
}

void GameObject::removeBody(int meshIndex)
{
	if (m_meshes[meshIndex].body)
	{
		BulletPhysics::getInstance()->removeObject(m_meshes[meshIndex].body);
		m_meshes[meshIndex].body = nullptr;
	}
}

void GameObject::setShouldRender(bool condition)
{
	m_shouldRender = condition;
}

void GameObject::setMaterial(Material* material, int meshIndex)
{
	if (meshIndex == -1)
	{
		// Special case to make all models use material of the first mesh
		for (int i = 0; i < (int)m_meshes.size(); i++)
		{
			if (m_meshes[i].mesh)
				m_meshes[i].material = m_meshes[0].material;
		}
	}
	else
	{
		if (m_meshes.size() >= meshIndex)
			m_meshes[meshIndex].material = material;
	}

}

Mesh* GameObject::getMesh(const int& meshIndex)
{
	return m_meshes[meshIndex].mesh;
}

Material* GameObject::getMaterial(const int& meshIndex)
{
	return m_meshes[meshIndex].material;
}

const Transform GameObject::getTransform(int meshIndex) const
{
	// Adds the inherited transforms together to get the world position of a mesh
	Transform world_transform;
	world_transform.position = m_transform.position + m_meshes[meshIndex].transform.position;
	world_transform.rotation = m_transform.rotation * m_meshes[meshIndex].transform.rotation;
	world_transform.scale = m_transform.scale * m_meshes[meshIndex].transform.scale;

	return world_transform;
}

const Transform GameObject::getObjectTransform() const
{
	return m_transform;
}

const Transform GameObject::getLocalTransform(int meshIndex) const
{
	return m_meshes[meshIndex].transform;
}

const Transform GameObject::getRigidTransform(int meshIndex) const
{
	if (m_meshes.size() == 0 || !m_meshes[meshIndex].body)
		return getObjectTransform();

	btVector3 rigidBodyPos = m_meshes[meshIndex].body->getWorldTransform().getOrigin();

	btTransform rigidBodyTransform = m_meshes[meshIndex].body->getWorldTransform();
	Transform newTransform;
	newTransform.position.x = rigidBodyTransform.getOrigin().getX();
	newTransform.position.y = rigidBodyTransform.getOrigin().getY();
	newTransform.position.z = rigidBodyTransform.getOrigin().getZ();

	newTransform.rotation.x = rigidBodyTransform.getRotation().getX();
	newTransform.rotation.y = rigidBodyTransform.getRotation().getY();
	newTransform.rotation.z = rigidBodyTransform.getRotation().getZ();
	newTransform.rotation.w = rigidBodyTransform.getRotation().getW();

	newTransform.scale = getTransform(meshIndex).scale;

	return newTransform;
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

void GameObject::bindMaterialToShader(std::string shaderName, int meshIndex)
{
	ShaderMap::getInstance()->getShader(shaderName)->setMaterial(m_meshes[meshIndex].material);
}

void GameObject::bindMaterialToShader(Shader* shader, const int& meshIndex)
{
	shader->setMaterial(m_meshes[meshIndex].material);
}

void GameObject::makeStatic()
{
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		const std::vector<Vertex>& vertices = m_meshes[i].mesh->getVertices();

		// Animated mesh case
		if (vertices.size() == 0)
		{
			const std::vector<Vertex2>& vertices2 = m_meshes[i].mesh->getVerticesSkele();

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

			m_meshes[i].body = BulletPhysics::getInstance()->createObject(
				box,
				0.0f,
				center,
				halfSize
			);

			m_meshes[i].body->setUserPointer(this);
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

			m_meshes[i].body = BulletPhysics::getInstance()->createObject(
				box,
				0.0f,
				center,
				halfSize,
				getTransform(i).rotation
			);

			m_meshes[i].body->setUserPointer(this);
		}
	}

	m_transform.position = glm::vec3(0.0f);
	m_transform.rotation = glm::quat();
}

void GameObject::createRigidBody(btRigidBody* body, int meshIndex)
{
	if (m_meshes.size() <= meshIndex)
	{
		MeshBox newBox;
		newBox.body = body;
		newBox.body->setUserPointer(this);

		m_meshes.emplace_back(newBox);
	}
	else
	{
		m_meshes[meshIndex].body = body;
		m_meshes[meshIndex].body->setUserPointer(this);
	}

}

void GameObject::createDynamic(CollisionObject shape, float weight, int meshIndex, bool recenter)
{
	if (m_meshes.size() <= meshIndex)
		m_meshes.resize(meshIndex + 1);

	const std::vector<Vertex>& vertices = m_meshes[meshIndex].mesh->getVertices();

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
	m_meshes[meshIndex].body = BulletPhysics::getInstance()->createObject(
		shape,
		weight,
		center,
		halfSize,
		getTransform(meshIndex).rotation,
		true,
		0.0f,
		1.0f);

	m_meshes[meshIndex].body->setUserPointer(this);
	m_meshes[meshIndex].body->setGravity(btVector3(0.0f, -25.0f, 0.0f));

	m_transform.position = glm::vec3(0.0f);
	m_transform.rotation = glm::quat();

	setTransformFromRigid(meshIndex);
}

void GameObject::updateBulletRigids()
{
	for (int i = 0; i < (int)m_meshes.size(); i++)
	{
		setTransformFromRigid(i);
	}
}

void GameObject::setTransformFromRigid(int meshIndex)
{
	if (!m_meshes[meshIndex].body)
		return;

	const btTransform& rigidBodyTransform = m_meshes[meshIndex].body->getWorldTransform();
	const btVector3& btPos = rigidBodyTransform.getOrigin();
	const btQuaternion& btRotation = rigidBodyTransform.getRotation();

	t_transform.position.x = btPos.getX();
	t_transform.position.y = btPos.getY();
	t_transform.position.z = btPos.getZ();

	t_transform.rotation.x = btRotation.getX();
	t_transform.rotation.y = btRotation.getY();
	t_transform.rotation.z = btRotation.getZ();
	t_transform.rotation.w = btRotation.getW();

	setMeshOffsetPosition(t_transform.position, meshIndex);
	setMeshOffsetRotation(t_transform.rotation, meshIndex);
}

void GameObject::addParticle(ParticleBuffers* particleBuffers)
{
	m_particleSystems.emplace_back(ParticleSystem(particleBuffers));
}

void GameObject::UpdateParticles(float dt)
{
	//for (ParticleSystem system : m_particleSystems)
	//{
	//	system.Update(camera->getCamPos(), dt);
	//}

	for (int i = 0; i < m_particleSystems.size(); i++)
	{
		m_particleSystems[i].Update(dt);
	}
}

void GameObject::UpdateParticles(float dt, PSinfo psInfo)
{
	//for (ParticleSystem system : m_particleSystems)
	//{
	//	system.Update(camera->getCamPos(), dt);
	//}

	for (int i = 0; i < m_particleSystems.size(); i++)
	{
		m_particleSystems[i].TempInit(psInfo);
		m_particleSystems[i].Update(dt);
	}
}

void GameObject::RenderParticles(Camera* camera)
{
	for (int i = 0; i < m_particleSystems.size(); i++)
	{
		m_particleSystems[i].SetPosition(m_transform.position);
		m_particleSystems[i].Render(camera);
	}
}

void GameObject::RemoveParticle()
{
	removeParticle = true;
}

bool GameObject::ShouldDie()
{
	return removeParticle;
}
