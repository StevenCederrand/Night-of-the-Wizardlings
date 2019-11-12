#include <Pch/Pch.h>
#include "DeflectRender.h"


DeflectRender::DeflectRender()
{
}

DeflectRender::~DeflectRender()
{
	glDeleteVertexArrays(1, &m_buffer.VAO);
	glDeleteBuffers(1, &m_buffer.VBO);
}

const GLuint& DeflectRender::getVAO() const
{
	return m_buffer.VAO;
}

void DeflectRender::prepareBuffers()
{
	glGenVertexArrays(1, &m_buffer.VAO);
	glGenBuffers(1, &m_buffer.VBO);

	glBindVertexArray(m_buffer.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(deflectBox), &deflectBox, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
}

glm::mat4 DeflectRender::getModelMatrix() const
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(10.0f, 5.0f, 1.0f));
	model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
	model = glm::rotate(model, glm::degrees(90.0f), glm::vec3(1, 0, 0));
	return model;
}
