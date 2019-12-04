#ifndef _TEXT_MANAGER_H
#define _TEXT_MANAGER_H
#include <Pch/Pch.h>
#include "FontType.h"
#include "GUIText.h"

class TextManager {
public:
	// Setup:
	// StartBehaviour_EndBehaviour

	enum TextBehaviour {
		ScaleUp_FadeOut,
		ScaleUp_ScaleDown,
		FadeIn_FadeOut,
	};

	struct TextEntity {
		GUIText* text = nullptr;
		glm::vec3 velocity;
		float lifeTime = 0.0f;
		float currentLifeTime = 0.0f;
		TextBehaviour behaviour;
	};

public:
	TextManager();
	~TextManager();
	void cleanup();
	
	void update();
	static TextManager* getInstance();


	GUIText* addDynamicText(const std::string& text, const float& fontSize, const glm::vec3& position, const float& lifetime, TextBehaviour behaviour, const glm::vec3& velocity = glm::vec3(0.0f));


private:
	

	std::vector<TextEntity> m_textEntities;
	FontType* m_fontType;

};


#endif