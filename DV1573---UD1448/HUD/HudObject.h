#ifndef _HUD_OBJECT_H
#define _HUD_OBJECT_H
#include <Pch/Pch.h>




class HudObject {

public:
	HudObject(const std::string& texturePath, const glm::vec2& position, 
		const glm::vec2& size = glm::vec2(1.0f), const glm::quat& rotation = glm::quat());
	~HudObject();

	/* SET FUNCTIONS */
	void setPosition(const glm::vec2& position);
	void setScale(const glm::vec2& scale);
	void setRotation(const glm::quat& rotation);

	void setAlpha(const float& alpha);
	void setXClip(const float& xClip);
	void setYClip(const float& yClip);
	void setFillColor(const glm::vec3& color);
	void setGrayscale(const int& grayscale);

	/* GET FUNCTIONS */
	const glm::mat4& getModelMatrix() const;
	const GLuint& getVAO() const;
	const GLuint& getVBO() const;
	const GLuint& getTextureID() const;
	const float& getAlpha() const;
	const float& getXClip() const;
	const float& getYClip() const;
	const glm::vec3& getFillColor() const;
	const int& getGrayscale() const;
	const glm::quat& getRotation() const;

private:
	void setupBuffers();
	void loadTexture(const std::string& texturePath);
	void updateModelMatrix();
	void setCenter();

private:
	GLuint m_vao, m_vbo, m_textureID;
	glm::vec2 m_position, m_size;
	glm::quat m_rotation;
	glm::mat4 m_modelMatrix;
	glm::vec3 m_fillColor; //For bars
	glm::vec2 m_centerPosition; //Center position of the sprite
	int m_grayscale;
	float m_alpha;
	float m_xClip;	//Clip the texture uv -- Can be used for other things, but mainly for HP
	float m_yClip;	//Clip the texture uv -- Can be used for other things, but mainly for HP
	float quadVertices[16] = {
		// positions        // texture Coords
		 -1.0, -1.0,		0.0f, 1.0f,
		  1.0, -1.0,		1.0f, 1.0f, //Bottom Right
		 -1.0,  1.0,		0.0f, 0.0f, //Top Right
		  1.0,  1.0,		1.0f, 0.0f
	};

};


#endif
