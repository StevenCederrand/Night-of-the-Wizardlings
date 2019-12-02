#ifndef _WORLD_HUD_OBJECT_H
#define _WORLD_HUD_OBJECT_H
#include <Pch/Pch.h>

class WorldHudObject {
public:

	WorldHudObject(const std::string& texturePath, const glm::vec3& center,
		const glm::vec2& scale = glm::vec2(1.0f));
	~WorldHudObject();

	/* Setters */
	void setCenter(const glm::vec3& center);
	void setScale(const glm::vec2& scale);
	
	void setAlpha(const float& alpha);
	void setXClip(const float& xClip);
	void setYClip(const float& yClip);
	void setShouldRender(bool condition);

	/* Getters */
	const GLuint& getVAO() const;
	const GLuint& getTextureID() const;
	const float& getAlpha() const;
	const float& getXClip() const;
	const float& getYClip() const;
	const glm::vec3& getCenter() const;
	const glm::vec2& getScale() const;
	const bool& getShouldRender() const;
private:
	void setupBuffers();
	void loadTexture(const std::string& texturePath);

private:
	GLuint m_vao, m_vbo, m_textureID;
	glm::vec3 m_center;
	glm::vec2 m_scale;
	float m_alpha;
	float m_xClip;	//Clip the texture uv -- Can be used for other things, but mainly for HP
	float m_yClip;	//Clip the texture uv -- Can be used for other things, but mainly for HP
	bool m_shouldRender;
	GLfloat m_vertexData[20] = {
		// Position				// Texture
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f
	};

};


#endif
