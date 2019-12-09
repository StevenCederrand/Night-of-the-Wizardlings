#include <Pch/Pch.h>
#include "TextMeshCreator.h"
#include "GUIText.h"

TextMeshCreator::TextMeshCreator(const std::string& fontFile)
{
	m_metafile = new Metafile();
	if (!m_metafile->Load(fontFile)) {
		logError("Font file could not be found: {0}", fontFile.c_str());
	}
}

TextMeshCreator::~TextMeshCreator()
{
	delete m_metafile;
}

TextMeshData TextMeshCreator::createTextMesh(GUIText* text)
{
	TextMeshData meshData = createQuadVertices(text);
	return meshData;
}


TextMeshData TextMeshCreator::createQuadVertices(GUIText* text)
{

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	
	vertices.reserve(100);
	uvs.reserve(100);

	Word currentWord = Word(text->getFontSize());
	for (size_t i = 0; i < text->getText().size(); i++) {
		int ascii = (int)text->getText()[i];
		if (ascii == SPACE_ASCII) {
			Character c;
			c.id = ascii;
			currentWord.addCharacter(c);
			continue;
		}
		currentWord.addCharacter(m_metafile->getCharacter(ascii));
	}
	float cursorX = 0.5f - currentWord.getWordWidth() * 0.5f;
	
	for (const Character& c : currentWord.getCharacters()) {
		if (c.id == SPACE_ASCII) {
			cursorX += m_metafile->getSpaceWidth() * text->getFontSize();
			continue;
		}

		addVerticesForCharacter(cursorX, 0.0f, c, text->getFontSize(), vertices);
		addUvs(uvs, c.textureCoord.x, c.textureCoord.y, c.textureCoord.z, c.textureCoord.w);
		cursorX += c.xAdvance * text->getFontSize();
	}
		

	TextMeshData tMesh;
	tMesh.vertexPositions = vertices;
	tMesh.textureCoords = uvs;
	tMesh.totalWordWith = currentWord.getWordWidth();
	return tMesh;
}

void TextMeshCreator::addVerticesForCharacter(float cursorX, float cursorY, Character c, float fontSize, std::vector<glm::vec3>& vertices)
{
	float x = cursorX + (c.offset.x * fontSize);
	float y = cursorY + (c.offset.y * fontSize);
	float maxX = x + (c.size.x * fontSize);
	float maxY = y + (c.size.y * fontSize);
	float properX = (2 * x) - 1;
	float properY = (-2 * y) + 1;
	float properMaxX = (2 * maxX) - 1;
	float properMaxY = (-2 * maxY) + 1;
	addVertices(vertices, properX, properY, properMaxX, properMaxY);

}

void TextMeshCreator::addVertices(std::vector<glm::vec3>& vertices, float x, float y, float maxX, float maxY)
{
	vertices.emplace_back(x, y, 0.0f);
	vertices.emplace_back(x, maxY , 0.0f);
	vertices.emplace_back(maxX, maxY, 0.0f);
	vertices.emplace_back(maxX, maxY, 0.0f);
	vertices.emplace_back(maxX, y, 0.0f);
	vertices.emplace_back(x, y, 0.0f);
}

void TextMeshCreator::addUvs(std::vector<glm::vec2>& uvs, float x, float y, float maxX, float maxY)
{
	uvs.emplace_back(x,y);
	uvs.emplace_back(x, maxY);
	uvs.emplace_back(maxX, maxY);
	uvs.emplace_back(maxX, maxY);
	uvs.emplace_back(maxX, y);
	uvs.emplace_back(x, y);
}


  