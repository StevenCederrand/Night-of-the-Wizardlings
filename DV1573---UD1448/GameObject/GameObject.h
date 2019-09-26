#ifndef	_GAMEOBJECT_h
#define _GAMEOBJECT_h
#include <Pch/Pch.h>
#include <Mesh/Mesh.h>
#include <GFX/MaterialMap.h>
#include <Mesh/Mesh.h>

//TODO: MOVE
//Handles seperate transforms for same mesh
struct MeshBox
{
	std::string name;
	Transform transform;
};

class GameObject {
public:
	//Create an Empty object
	GameObject();
	//Create an Empty object with a different name
	GameObject(std::string objectName);
	virtual ~GameObject();

	//Loads all the meshes from the file into the GameObject
	void loadMesh(std::string fileName);
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
	Mesh* getMesh() const;
	Mesh* getMesh(int meshIndex) const;
	const std::string& getMeshN(int meshIndex) const;
	const int getMeshesCount() const { return (int)m_meshes.size(); }


private:

	std::string m_objectName;
	Transform m_transform;

	std::vector<MeshBox> m_meshes;

};


#endif