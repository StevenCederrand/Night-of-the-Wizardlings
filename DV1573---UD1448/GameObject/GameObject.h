#ifndef	_GAMEOBJECT_h
#define _GAMEOBJECT_h
#include <Pch/Pch.h>
#include <Mesh/Mesh.h>
#include <GFX/MaterialMap.h>
#include <Mesh/Mesh.h>
#include <System/BulletPhysics.h>
#include <Particles/Particles.h>
#include <Particles/ParticleBuffers.h>

class GameObject {
public:
	//Create an Empty object
	GameObject();
	//Create an Empty object with a different name
	GameObject(std::string objectName);
	virtual ~GameObject();
	virtual void update(float dt) = 0;
	
	//Loads all the meshes from the file into the GameObject
	void loadMesh(std::string fileName);
	void initMesh(Mesh mesh);
	void initMesh(std::string name, std::vector<Vertex> vertices, std::vector<Face> faces);
	void bindMaterialToShader(std::string shaderName, int meshIndex = 0);
	void bindMaterialToShader(Shader* shader, const int& meshIndex = 0);
	
	// Bullet
	void makeStatic();
	void updateBulletRigids();
	void setTransformFromRigid(int meshIndex = 0);
	void createRigidBody(btRigidBody* body, int meshIndex = 0);
	void createDynamic(CollisionObject shape = box, float weight = 1.0f, int meshIndex = 0, bool recenter = true);	
	void removeBody(int bodyIndex);
	void setBodyActive(bool state = false, int meshIndex = 0);

	// Particles
	void addParticle(ParticleBuffers particleBuffers);

	void setTransform(Transform transform);
	void setTransform(glm::vec3 worldPosition = glm::vec3(), glm::quat worldRot = glm::quat(), glm::vec3 worldScale = glm::vec3(1.0f));
	void setMeshOffsetTransform(Transform transform, int meshIndex);
	void setWorldPosition(glm::vec3 worldPosition);
	void setMeshOffsetPosition(glm::vec3 worldPosition, int meshIndex);
	void setBodyWorldPosition(glm::vec3 worldPosition, int meshIndex);

	void setShouldRender(bool condition);
	void setMaterial(Material* material, int meshIndex = 0);

	//Get functions
	Mesh* getMesh(const int& meshIndex = 0); //Get a mesh from the meshbox
	Material* getMaterial(const int& meshIndex = 0); //Get a material from the meshbox

	const Transform getTransform(int meshIndex) const;
	const Transform getObjectTransform() const;
	const Transform getLocalTransform(int meshIndex) const;
	const Transform getRigidTransform(int meshIndex) const;
	const int getMeshesCount() const { return (int)m_meshes.size(); }
	const glm::mat4& getMatrix(const int& i = 0) const;
	const int getType() const { return m_type; }
	const bool& getShouldRender() const;
	const glm::vec3 getLastPosition() const;
	const std::vector<ParticleSystem>& getParticles() const { return m_particleSystems; } //Get the particle vector so we can render and stuff

	btRigidBody* getRigidBody(int meshIndex = 0) const { return m_meshes[meshIndex].body; }

private:
	void updateTransform();

	struct MeshBox //Handles seperate transforms for same mesh
	{
		Mesh* mesh;
		Material* material;
		Transform transform;

		btRigidBody* body;

		MeshBox()
		{
			mesh = nullptr;
			material = nullptr;
			body = nullptr;
		}
	};

	std::string m_objectName;
	Transform m_transform;
	glm::vec3 m_lastPosition;

	// Allocation for perfomance
	Transform t_transform;

	std::vector<ParticleSystem> m_particleSystems;

protected:
	std::vector<glm::mat4> m_modelMatrixes;
	std::vector<MeshBox> m_meshes;
	bool m_shouldRender;
	int m_type;
};


#endif