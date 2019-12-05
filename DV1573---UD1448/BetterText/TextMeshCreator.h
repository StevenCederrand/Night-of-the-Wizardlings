#ifndef _TEXT_MESH_CREATOR_H
#define _TEXT_MESH_CREATOR_H
#include <Pch/Pch.h>
#include "TextMeshData.h"
#include "Metafile.h"
#include "Line.h"

class GUIText;


class TextMeshCreator {

public:
	TextMeshCreator(const std::string& fontFile);
	~TextMeshCreator();

	TextMeshData createTextMesh(GUIText* text);

private:
	std::vector<Line> createStructure(GUIText* text);
	void completeStructure(std::vector<Line>& lines, Line currentLine, Word currentWord, GUIText* text);
	TextMeshData createQuadVertices(GUIText* text, const std::vector<Line>& lines);
	void addVerticesForCharacter(float cursorX, float cursorY, Character c, float fontSize, std::vector<glm::vec3>& vertices);

	void addVertices(std::vector<glm::vec3>& vertices, float x, float y, float maxX, float maxY);
	void addUvs(std::vector<glm::vec2>& uvs, float x, float y, float maxX, float maxY);

private:
	Metafile* m_metafile;

};


#endif
