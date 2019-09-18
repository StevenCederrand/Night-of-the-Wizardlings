#ifndef	_GAMEOBJECT_h
#define _GAMEOBJECT_h
#include <Pch/Pch.h>
#include <Mesh/Mesh.h>
#include <GFX/MaterialMap.h>

struct Transform {
	glm::vec3 m_worldPos;
	glm::quat m_worldRot;
	glm::vec3 m_worldScale;
};

class GameObject {
public:
	//Create an Empty object
	GameObject();
	//Create an Empty object with a different name
	GameObject(std::string objectName);
	~GameObject();

	//Automatically adds MESHPATH, to the name
	void loadMesh(std::string meshName);
	//Bind all of the material values to the shader, i.e colors
	void bindMaterialToShader(std::string shaderName);


	//Set functions
	void setTransform(Transform transform);
	void setTransform(glm::vec3 worldPosition, glm::quat worldRot, glm::vec3 worldScale);
	void setWorldPosition(glm::vec3 worldPosition);
	//Get functions
	const Transform& getTransform() const;
	Mesh* getMesh() const;


private:
	std::string m_objectName;

	Mesh* m_mesh = nullptr;
	std::string m_materialName;

	Transform m_transform;

};


#endif