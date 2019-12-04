#include "Pch/Pch.h"
#include "GUIText.h"
#include "FontType.h"
#include <System/UniqueIndex.h>
#include <Renderer/Camera.h>

GUIText::GUIText(const std::string& text, float fontSize, FontType* fontType, const glm::vec3& position, float maxLineLength, bool centered)
{
	m_text = text;
	m_fontSize = fontSize;
	m_fontType = fontType;
	m_position = position;
	m_lineMaxSize = maxLineLength;
	m_centered = centered;
	m_color = glm::vec4(1.0f);
	m_rotationMatrix = glm::mat4(1.0f);
	m_modelMatrix = glm::mat4(1.0f);
	m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(m_position));

	createBuffers();
	
	m_uniqueIndex = UniqueIndex::getUniqueIndex();
}

GUIText::~GUIText()
{
	remove();
}

void GUIText::setColor(const glm::vec4& color)
{
	m_color = color;
}

void GUIText::remove()
{
	deleteBuffers();
}

void GUIText::setMeshInfo(int vao, int verticesCount)
{
	m_textMeshVao = vao;
	m_vertexCount = verticesCount;
}

void GUIText::setNumberOfLines(int number)
{
	m_numberOfLines = number;
}

void GUIText::setScale(const float& scale)
{
	m_scale = scale;
}

void GUIText::setInitialScale(const float& scale)
{
	m_initialScale = scale;
}

void GUIText::setPosition(const glm::vec3 position)
{
	m_position = position;
}

void GUIText::rotateTowardsCamera(Camera* camera)
{
	glm::vec3 dir = glm::normalize(m_position - camera->getCamPos());
	m_rotationMatrix = glm::inverse(glm::lookAt(glm::vec3(0.0f), camera->getCamFace(), glm::vec3(0, 1, 0)));
}

const unsigned int& GUIText::getUniqueIndex() const
{
	return m_uniqueIndex;
}

const int& GUIText::getNumberOfLines() const
{
	return m_numberOfLines;
}

const unsigned int& GUIText::getMeshVao() const
{
	return m_textMeshVao;
}

const int& GUIText::getVertexCount() const
{
	return m_vertexCount;
}

const float& GUIText::getMaxLineSize() const
{
	return m_lineMaxSize;
}

const float& GUIText::getCurrentScale() const
{
	return m_scale;
}

const float& GUIText::getInitialScale() const
{
	return m_initialScale;
}

const bool GUIText::getFaceCamera() const
{
	return m_faceCamera;
}

const float& GUIText::getFontSize() const
{
	return m_fontSize;
}

const bool GUIText::isCentered() const
{
	return m_centered;
}

const glm::vec3& GUIText::getPosition() const
{
	return m_position;
}

const glm::vec4& GUIText::getColor() const
{
	return m_color;
}

const glm::mat4& GUIText::getModelMatrix() const
{
	return m_modelMatrix;
}

FontType* GUIText::getFontType() const
{
	return m_fontType;
}

const std::string& GUIText::getText() const
{
	return m_text;
}

void GUIText::updateModelMatrix()
{
	m_modelMatrix = glm::mat4(1.0f);
	m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(m_position));
	m_modelMatrix = m_modelMatrix * m_rotationMatrix;
	m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(m_scale, m_scale, 1.0f));
}

void GUIText::setAlpha(const float& alpha)
{
	m_color.a = alpha;
}

void GUIText::setToFaceCamera(bool condition)
{
	m_faceCamera = condition;
}

void GUIText::createBuffers()
{
	//m_timer.start();

	TextMeshData data = m_fontType->loadText(this);

	// setup plane VAO
	glGenVertexArrays(1, &m_textMeshVao);
	glGenBuffers(2, m_buffers);

	glBindVertexArray(m_textMeshVao);

	// ---------------------------Pos---------------------------
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * data.vertexPositions.size(), &data.vertexPositions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	//---------------------------Uvs---------------------------
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * data.textureCoords.size() , &data.textureCoords[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	// Unbind the vao
	glBindVertexArray(NULL);

	m_vertexCount = data.vertexPositions.size();
	
	//m_timer.stop();

	//std::printf("GUIText Creation: \n");
	//m_timer.print();

}

void GUIText::deleteBuffers()
{
	glDeleteBuffers(2, m_buffers);
	glDeleteVertexArrays(1, &m_textMeshVao);
}
