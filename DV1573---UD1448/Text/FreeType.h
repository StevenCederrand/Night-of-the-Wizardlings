#ifndef _FREETYPE_h
#define _FREETYPE_h
#include <Pch/Pch.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Text/HUDShader.h>

struct TextCharacter {
	GLuint TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	GLuint Advance;
};
//std::map<GLchar, TextCharacter> TextCharacters;

class FreeType
{
private:

	GLuint VAO, VBO;
public:
	std::map<GLchar, TextCharacter> TextCharacters;
	HUDShader* m_hudShader;

	FreeType(glm::mat4 camMat);
	~FreeType();
	void BindTexture();
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale,
		glm::vec3 color);
};


#endif // !_TextClass_



