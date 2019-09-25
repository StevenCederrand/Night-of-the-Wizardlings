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
	virtual ~GameObject();

	//Automatically adds MESHPATH, to the name
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
	const Transform& getTransform() const;
	Mesh* getMesh() const;
	Mesh* getMesh(int index) const;
	const std::vector<Mesh*>& getMeshes() const;


private:

	std::string m_objectName;

	std::vector<Mesh*> m_meshes;
	std::vector<std::string> m_materialNames;

	Transform m_transform;
};


#endif