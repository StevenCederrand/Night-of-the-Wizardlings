#ifndef _FONT_TYPE_H
#define _FONT_TYPE_H
#include <Pch/Pch.h>
#include "TextMeshCreator.h"
#include "TextMeshData.h"

class GUIText;



class FontType {

public:
	FontType(const std::string& fontTexture, const std::string& fontFile);
	~FontType();

	const unsigned int& getTextureAtlas() const;
	TextMeshData loadText(GUIText* text);

private:
	bool loadTexture(const std::string& fontTexture);

private:
	unsigned int m_textureAtlas;
	TextMeshCreator* m_textMeshCreator;

};



#endif