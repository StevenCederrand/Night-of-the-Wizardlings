#ifndef _CUBE_h
#define _CUBE_h
#include <Pch/Pch.h>
class Cube {
public:
	Cube(GLuint& vbo);
	~Cube();

	GLuint getVAO() const;
	glm::vec3 getWorldPos() const;
	glm::mat4 getModelMatrix();

private:
	GLuint m_VAO;
	glm::vec3 m_worldPos;
	glm::mat4 m_modelMatrix;
};



#endif