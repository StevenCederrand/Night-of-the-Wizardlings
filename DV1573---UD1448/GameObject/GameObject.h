#ifndef	_GAMEOBJECT_h
#define _GAMEOBJECT_h
#include <Pch/Pch.h>
#include <Mesh/Mesh.h>
#include <GFX/MaterialMap.h>
#include <Mesh/Mesh.h>
#include <System/BulletPhysics.h>
#include <Particles/Particles.h>
#include <Particles/ParticleBuffers.h>
#include <Renderer/Camera.h>

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
	void bindMaterialToShader(std::string shaderName);
	void bindMaterialToShader(std::string shaderName, int meshIndex);
	void bindMaterialToShader(Shader* shader, const int& meshIndex);
	void bindMaterialToShader(Shader* shader, const std::string& materialName);
	void bindMaterialToShader(Shader* shader, Material* material);
	void unbindMaterialFromShader(Shader* shader, const std::string& materialName);
	void unbindMaterialFromShader(Shader* shader, Material* material);
	
	//Create a rigid body of the shape of your choice and add it to the collision world
	void createRigidBody(CollisionObject shape);
	void createRigidBody(btRigidBody* body);
	void createDynamicRigidBody(CollisionObject shape, float weight);
	void createDynamicRigidBody(CollisionObject shape, float weight, int meshIndex, bool recenter = true);	
	void updateBulletRigids();

	void setTransformFromRigid(int i);
	void addParticle(ParticleBuffers* particleBuffers);
	void UpdateParticles(Camera* camera, float dt);

	virtual void update(float dt) = 0;
	   	
	//Set functions
	void setTransform(Transform transform);
	void setTransform(Transform transform, int meshIndex);
	void setTransform(glm::vec3 worldPosition, glm::quat worldRot, glm::vec3 worldScale);
	void setBtOffset(glm::vec3 offset, int meshIndex);
	void setWorldPosition(glm::vec3 worldPosition);
	void setWorldPosition(glm::vec3 worldPosition, int meshIndex);
	void offsetMesh(glm::vec3 position, int meshIndex);
	void setBTWorldPosition(glm::vec3 worldPosition, int meshIndex);
	void setBTTransform(Transform transform, int meshIndex);
	void set_BtActive(bool state = false, int meshIndex = 0);
	void removeBody(int bodyIndex);
	void translate(const glm::vec3& translationVector);
	void setShouldRender(bool condition);
	void setMaterial(std::string matName, int meshIndex = -1);

	//Get functions
	const Transform getTransform() const;
	Material* getMaterial(const int& meshIndex); //Get a material from the meshbox
	Mesh* getMesh(const int& meshIndex); //Get a mesh from the meshbox
	const Transform getTransform(int meshIndex) const;
	const Transform& getTransform(Mesh* mesh, const int& meshIndex) const;
	const Transform getTransformMesh(int meshIndex) const;
	const Transform getTransformRigid(int meshIndex) const;
	const std::string& getMeshName(int meshIndex = 0) const;
	const int getMeshesCount() const { return (int)m_meshes.size(); }
	const glm::mat4& getMatrix(const int& i) const;
	const int getType() const { return m_type; }
	const bool& getShouldRender() const;
	const glm::vec3 getLastPosition() const;
	const std::vector<ParticleSystem>& getParticles() const { return m_particleSystems; } //Get the particle vector so we can render and stuff

	std::vector<btRigidBody*> getRigidBodies() { return m_bodies; }	
	btRigidBody* getRigidBody() const { return m_bodies[0]; }	

private:
	void updateModelMatrix();
	struct MeshBox //Handles seperate transforms for same mesh
	{
		std::string name; //This is kinda useless 
		Transform transform;
		Material* material;
		Mesh* mesh;
		glm::vec3 btoffset = glm::vec3(0.0f);
	};

	std::string m_objectName;
	Transform m_transform;
	glm::vec3 m_lastPosition;

	std::vector<btRigidBody*> m_bodies;	

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