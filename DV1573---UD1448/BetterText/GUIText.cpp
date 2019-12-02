#include "Pch/Pch.h"
#include "GUIText.h"
#include "FontType.h"
#include <System/UniqueIndex.h>

GUIText::GUIText(const std::string& text, float fontSize, FontType* fontType, const glm::vec3& position, float maxLineLength, bool centered)
{
	m_text = text;
	m_fontSize = fontSize;
	m_fontType = fontType;
	m_position = position;
	m_lineMaxSize = maxLineLength;
	m_centered = centered;

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

const int& GUIText::getMaxLineSize() const
{
	return m_lineMaxSize;
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

FontType* GUIText::getFontType() const
{
	return m_fontType;
}

const std::string& GUIText::getText() const
{
	return m_text;
}

void GUIText::createBuffers()
{
	TextMeshData data = m_fontType->loadText(this);

	// setup plane VAO
	glGenVertexArrays(1, &m_textMeshVao);
	glGenBuffers(2, m_buffers);

	glBindVertexArray(m_textMeshVao);

	// ---------------------------Pos---------------------------
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data.vertexPositions.data()), data.vertexPositions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	//---------------------------Uvs---------------------------
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data.textureCoords.data()), data.textureCoords.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	// Unbind the vao
	glBindVertexArray(NULL);

	m_vertexCount = data.vertexPositions.size();
	
}

void GUIText::deleteBuffers()
{
	glDeleteBuffers(2, m_buffers);
	glDeleteVertexArrays(1, &m_textMeshVao);
}
