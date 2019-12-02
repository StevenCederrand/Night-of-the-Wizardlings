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
	auto lines = createStructure(text);
	TextMeshData meshData = createQuadVertices(text, lines);
	return meshData;
}

std::vector<Line> TextMeshCreator::createStructure(GUIText* text)
{
	std::vector<Line> lines;
	lines.reserve(10);

	Line currentLine(m_metafile->getSpaceWidth(), text->getFontSize(), text->getMaxLineSize());
	Word currentWord(text->getFontSize());

	auto vec = text->getText();

	for (char c : vec) {
		int ascii = (int)c;

		if (c == SPACE_ASCII) {
			boolean added = currentLine.attemptToAddWord(currentWord);

			if (added == false) {
				lines.push_back(currentLine);
				currentLine = Line(m_metafile->getSpaceWidth(), text->getFontSize(), text->getMaxLineSize());
				currentLine.attemptToAddWord(currentWord);

			}

			currentWord = Word(text->getFontSize());
			continue;
		}

		Character c = m_metafile->getCharacter(ascii);
		currentWord.addCharacter(c);
	}

	completeStructure(lines, currentLine, currentWord, text);

	return lines;
}

void TextMeshCreator::completeStructure(std::vector<Line>& lines, Line currentLine, Word currentWord, GUIText* text)
{
	bool added = currentLine.attemptToAddWord(currentWord);
	if (added == false) {
		lines.push_back(currentLine);
		currentLine = Line(m_metafile->getSpaceWidth(), text->getFontSize(), text->getMaxLineSize());
		currentLine.attemptToAddWord(currentWord);
	}

	lines.push_back(currentLine);

}

TextMeshData TextMeshCreator::createQuadVertices(GUIText* text, const std::vector<Line>& lines)
{
	text->setNumberOfLines(lines.size());
	float cursorX = 0;
	float cursorY = 0;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	
	vertices.reserve(100);
	uvs.reserve(100);

	for (const Line& line : lines) {
		if (text->isCentered()) {
			cursorX = (line.getMaxLength() - line.getCurrentLength()) * 0.5f;
		}

		for (const Word& word : line.getWords()) {
			for (const Character& c : word.getCharacters()) {

				addVerticesForCharacter(cursorX, cursorY, c, text->getFontSize(), vertices);
				addUvs(uvs, c.textureCoord.x, c.textureCoord.y, c.textureCoord.z, c.textureCoord.w);
				cursorX += c.xAdvance * text->getFontSize();
			}

			cursorX += m_metafile->getSpaceWidth() * text->getFontSize();

		}

		cursorX = 0;
		cursorY += LINE_HEIGHT * text->getFontSize();
	}


	TextMeshData tMesh;
	tMesh.vertexPositions = vertices;
	tMesh.textureCoords = uvs;

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
