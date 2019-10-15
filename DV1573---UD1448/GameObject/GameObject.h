#ifndef	_GAMEOBJECT_h
#define _GAMEOBJECT_h
#include <Pch/Pch.h>
#include <Mesh/Mesh.h>
#include <GFX/MaterialMap.h>
#include <Mesh/Mesh.h>
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
	void genBullet(BulletPhysics* bPhysics);
	//Bind all of the material values to the shader, i.e colors
	void bindMaterialToShader(std::string shaderName);
	void bindMaterialToShader(std::string shaderName, int matIndex);

	virtual void update(float dt) = 0;
	   	
	//Set functions
	void setTransform(Transform transform);
	void setTransform(glm::vec3 worldPosition, glm::quat worldRot, glm::vec3 worldScale);
	void setWorldPosition(glm::vec3 worldPosition);
	void translate(const glm::vec3& translationVector);

	//Get functions
	const Transform getTransform() const;
	//Returns mesh worldposition
	const Transform getTransform(int meshIndex) const;
	const std::string& getMeshName(int meshIndex) const;
	const int getMeshesCount() const { return (int)m_meshes.size(); }
	const const glm::mat4& getMatrix(int i) const;
	const int getType() const { return type; }
	

private:
	void updateModelMatrix();
	struct MeshBox //Handles seperate transforms for same mesh
	{
		std::string name;
		Transform transform;
	};

	std::string m_objectName;
	Transform m_transform;
	BulletPhysics* m_bPhysics;

protected:
	std::vector<glm::mat4> m_modelMatrixes;
	std::vector<MeshBox> m_meshes;
	int type;
};


#endif