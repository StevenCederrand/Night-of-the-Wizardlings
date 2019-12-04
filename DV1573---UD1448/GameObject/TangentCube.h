#ifndef TANGENTCUBE_H
#define TANGENTCUBE_H
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
class TangentCube : public GameObject
{
public:
	TangentCube();
	~TangentCube();

	void loadTexture();

	const GLuint& getNormalMapTexture() const;

	void update(float dt);
private:
	GLuint m_normalMap;
};

#endif