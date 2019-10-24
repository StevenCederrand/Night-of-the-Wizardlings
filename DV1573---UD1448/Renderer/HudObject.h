#ifndef _HUD_OBJECT_H
#define _HUD_OBJECT_H
#include <Pch/Pch.h>

class HudObject {

public:
	HudObject(const std::string& texturePath, const glm::vec2& position, const glm::vec2& size = glm::vec2(1.0f));
	~HudObject();

	void setPosition(const glm::vec3& position);
	void setScale(const glm::vec3& scale);
	void setAlpha(const float& alpha);

	const glm::mat4& getModelMatrix() const;

	const GLuint& getVAO() const;
	const GLuint& getVBO() const;
	const GLuint& getTextureID() const;
	const float& getAlpha() const;

private:
	void setupBuffers();
	void loadTexture(const std::string& texturePath);
	void updateModelMatrix();

private:
	GLuint m_vao, m_vbo, m_textureID;
	glm::vec2 m_position, m_size;
	glm::mat4 m_modelMatrix;
	float m_alpha;

	float quadVertices[16] = {
		// positions        // texture Coords
		 -1.0, -1.0,		0.0f, 1.0f,
		  1.0, -1.0,		1.0f, 1.0f,
		 -1.0,  1.0,		0.0f, 0.0f,
		  1.0,  1.0,		1.0f, 0.0f
	};

};


#endif
