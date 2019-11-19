#ifndef	_GAMEOBJECT_h
#define _GAMEOBJECT_h
#include <Pch/Pch.h>
#include <Mesh/Mesh.h>
#include <GFX/MaterialMap.h>
#include <Mesh/Mesh.h>
#include <DebugDrawer/DebugDrawer.h>
#include <System/BulletPhysics.h>

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
	void bindMaterialToShader(Shader* shader, Material* materialName);
	//Create a rigid body of the shape of your choice and add it to the collision world
	void createRigidBody(CollisionObject shape, BulletPhysics* bp);
	void createDynamicRigidBody(CollisionObject shape, BulletPhysics* bp, float weight);
	void createDynamicRigidBody(CollisionObject shape, BulletPhysics* bp, float weight, int meshIndex, bool recenter = true);
	void createDebugDrawer();
	void updateBulletRigids();

	void setTransformFromRigid(int i);


	virtual void update(float dt) = 0;
	   	
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
	void translate(const glm::vec3& translationVector);
	void setShouldRender(bool condition);
	void setMaterial(std::string matName, int meshIndex = -1);

	//Get functions
	const Transform getTransform() const;
	//Returns mesh worldposition
	const Transform getTransform(int meshIndex) const;
	const Transform& getTransform(Mesh* mesh, const int& meshIndex) const;
	const Transform getTransformMesh(int meshIndex) const;
	const Transform getTransformRigid(int meshIndex) const;
	const std::string& getMeshName(int meshIndex = 0) const;
	const int getMeshesCount() const { return (int)m_meshes.size(); }
	const glm::mat4& getMatrix(const int& i) const;
	const int getType() const { return m_type; }
	const std::vector<btRigidBody*>& getRigidBodies()  { return m_bodies; }
	const std::vector<DebugDrawer*>& getDebugDrawers()  { return m_debugDrawers; }
	const bool& getShouldRender() const;
	const glm::vec3 getLastPosition() const;

private:
	void updateModelMatrix();
	struct MeshBox //Handles seperate transforms for same mesh
	{
		std::string name;
		Transform transform;
	};

	std::string m_objectName;
	Transform m_transform;
	glm::vec3 m_lastPosition;
	BulletPhysics* m_bPhysics;

	std::vector<btRigidBody*> m_bodies;
	std::vector<DebugDrawer*> m_debugDrawers;

	// Allocate for later
	Transform t_transform;

protected:
	std::vector<glm::mat4> m_modelMatrixes;
	std::vector<MeshBox> m_meshes;
	bool m_shouldRender;
	int m_type;
};


#endif