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
	
	//Loads all the meshes from the file into the GameObject
	void loadMesh(std::string fileName);
	void initMesh(Mesh mesh);
	void initMesh(std::string name, std::vector<Vertex> vertices, std::vector<Face> faces);
	//Bind all of the material values to the shader, i.e colors
	void bindMaterialToShader(std::string shaderName, int meshIndex = 0);
	void bindMaterialToShader(Shader* shader, const int& meshIndex = 0);
	
	void createRigidBody(btRigidBody* body, int meshIndex = 0);
	void makeStatic();
	void createDynamic(CollisionObject shape = box, float weight = 1.0f, int meshIndex = 0, bool recenter = true);	

	void addParticle(ParticleBuffers particleBuffers);

	virtual void update(float dt) = 0;
	void updateBulletRigids();

	//Set functions
	void setTransform(Transform transform);
	void setTransform(Transform transform, int meshIndex);
	void setTransform(glm::vec3 worldPosition, glm::quat worldRot, glm::vec3 worldScale);
	void setWorldPosition(glm::vec3 worldPosition);
	void setWorldPosition(glm::vec3 worldPosition, int meshIndex);
	void offsetMesh(glm::vec3 position, int meshIndex);

	void setBTWorldPosition(glm::vec3 worldPosition, int meshIndex);
	void setBTTransform(Transform transform, int meshIndex);
	void set_BtActive(bool state = false, int meshIndex = 0);
	void setTransformFromRigid(int meshIndex = 0);
	void removeBody(int bodyIndex);

	void translate(const glm::vec3& translationVector);
	void setShouldRender(bool condition);
	void setMaterial(Material* material, int meshIndex = 0);

	//Get functions
	const Transform getTransform() const;
	Material* getMaterial(const int& meshIndex); //Get a material from the meshbox
	Mesh* getMesh(const int& meshIndex = 0); //Get a mesh from the meshbox

	const Transform getTransform(int meshIndex) const;
	const Transform& getTransform(Mesh* mesh, const int& meshIndex) const;
	const Transform getTransformMesh(int meshIndex) const;
	const Transform getTransformRigid(int meshIndex) const;
	const int getMeshesCount() const { return (int)m_meshes.size(); }
	const glm::mat4& getMatrix(const int& i = 0) const;
	const int getType() const { return m_type; }
	const bool& getShouldRender() const;
	const glm::vec3 getLastPosition() const;
	const std::vector<ParticleSystem>& getParticles() const { return m_particleSystems; } //Get the particle vector so we can render and stuff

	btRigidBody* getRigidBody(int meshIndex = 0) const { return m_meshes[meshIndex].body; }

private:
	void updateModelMatrix();

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