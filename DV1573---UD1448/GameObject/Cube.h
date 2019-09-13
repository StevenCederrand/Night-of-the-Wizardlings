#ifndef _CUBE_h
#define _CUBE_h
#include <Pch/Pch.h>
class Cube {
public:
	Cube(GLuint vbo);
	~Cube();

	const GLuint& getVAO() const;
	const glm::vec3& getWorldPos() const;
	const glm::mat4& getModelMatrix() const;

private:
	GLuint m_VAO;
	glm::vec3 m_worldPos;
	glm::mat4 m_modelMatrix;
};



#endif