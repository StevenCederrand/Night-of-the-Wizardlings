#ifndef	_GAMEOBJECT_h
#define _GAMEOBJECT_h
#include <Pch/Pch.h>
#include <Mesh/Mesh.h>
#include <GFX/MaterialMap.h>


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
	void setTransform(glm::vec3 worldPosition, glm::quat worldRot = glm::quat(), glm::vec3 worldScale = glm::vec3(.0f));
	void setWorldPosition(glm::vec3 worldPosition);
	void translate(const glm::vec3& translationVector);

	//Get functions
	const Transform getTransform() const;
	//Returns mesh worldposition
	const Transform getTransform(int meshIndex) const;
	Mesh* getMesh() const;
	Mesh* getMesh(int meshIndex) const;
	const std::vector<Mesh*>& getMeshes() const;
	const int getMeshesCount() const { return (int)m_meshes.size(); }


private:

	std::string m_objectName;
	Transform m_transform;

	std::vector<Mesh*> m_meshes;

};


#endif