#ifndef _DEFLECT_RENDER_H
#define _DEFLECT_RENDER_H

#include <Pch/Pch.h>

struct DeflectBuffer
{
	GLuint VAO;
	GLuint VBO;
};

class DeflectRender
{
private:
	DeflectBuffer m_buffer;
	
	glm::vec3 deflectBox[6] =
	{
		// Temporary vertex positions          
		glm::vec3(-1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		 glm::vec3(1.0f, -1.0f, -1.0f),
		 glm::vec3(1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f)
	};

public:
	DeflectRender();
	~DeflectRender();

	const GLuint& getVAO() const;
	
	void prepareBuffers();
	glm::mat4 getModelMatrix() const;
};

#endif // !_DEFLECT_RENDER_H
