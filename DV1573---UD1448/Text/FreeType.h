#ifndef _FREETYPE_h
#define _FREETYPE_h
#include <Pch/Pch.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Text/HUDShader.h>
#include <Renderer/PickupNotificationStructure.h>

struct TextCharacter {
	GLuint TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	GLuint Advance;
};

class FreeType
{
private:

	GLuint VAO, VBO;
public:
	std::map<GLchar, TextCharacter> TextCharacters;
	HUDShader* m_hudShader;

	FreeType();
	~FreeType();
	void BindTexture();
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale,
		glm::vec3 color);

	void RenderText(PickupNotificationText notification, const glm::vec3& position, const glm::vec2& scale);
	unsigned int getTotalWidth(const std::string& text, const glm::vec3& scale);

};


#endif // !_TextClass_



