#include "Pch/Pch.h"
#include "TextManager.h"
#include <Renderer/TextRenderer.h>

TextManager::TextManager()
{
	m_fontType = new FontType("Assets/Fonts/DistFont.png", "Assets/Fonts/DistFont.fnt");
}

TextManager::~TextManager()
{
	delete m_fontType;
}

void TextManager::cleanup()
{
	for (auto& entry : m_textEntities) {
		delete entry.text;
	}
	m_textEntities.clear();
}

void TextManager::update()
{
	for(size_t i = 0; i < m_textEntities.size(); i++){
		TextEntity& t = m_textEntities[i];
		
		if (t.behaviour == TextBehaviour::StayForever) {
			TextRenderer::getInstance()->submitText(t.text);
			continue;
		}

		
		t.currentLifeTime += DeltaTime;
		t.text->setPosition(t.text->getPosition() + t.velocity * DeltaTime);
		
		float halfTime = t.lifeTime / 2.0f;
		bool endState = false;
		float percentage = 0.0f;
		
		if (t.currentLifeTime > halfTime) {
			// end behaviour
			endState = true;
			percentage = 1.0f - ((t.currentLifeTime - halfTime) / halfTime);
		}
		else if (t.currentLifeTime <= halfTime) {
			// start behaviour
			percentage = (t.currentLifeTime / halfTime);
		}

		
		switch (t.behaviour) {
			case TextBehaviour::FadeIn_FadeOut:
			{
				t.text->setAlpha(percentage);
				break;
			}

			case TextBehaviour::ScaleUp_FadeOut:
			{
				if (endState == false) {
					
					t.text->setScale(t.text->getInitialScale() * percentage);
				}
				else {

					t.text->setAlpha(percentage);
				}
				break;
			}


			case TextBehaviour::ScaleUp_ScaleDown:
			{
				t.text->setScale(t.text->getInitialScale() * percentage);
				break;
			}

			case TextBehaviour::Instant_FadOut:
			{
				if (endState == false) {
					t.text->setAlpha(1.0f);
					
				}
				else
				{
					t.text->setAlpha(percentage);
				}

				break;
			}

		}

		if (t.currentLifeTime >= t.lifeTime) {
			delete t.text;
			m_textEntities.erase(m_textEntities.begin() + i);
			i--;

		}
		else {
			TextRenderer::getInstance()->submitText(t.text);
		}

		

	}
}

TextManager* TextManager::getInstance()
{
	static TextManager t;
	return &t;
}

void TextManager::removeText(unsigned int uniqueKey)
{
	for (size_t i = 0; i < m_textEntities.size(); i++) {
		if (m_textEntities[i].text->getUniqueIndex() == uniqueKey) {
			delete m_textEntities[i].text;
			m_textEntities.erase(m_textEntities.begin() + i);
			return;
		}
	}
}

GUIText* TextManager::addDynamicText(const std::string& text, const float& fontSize, const glm::vec3& position, const float& lifetime, 
	TextBehaviour behaviour, const glm::vec3& velocity, bool ScreenText, const float& viewportWidth)
{
	GUIText* guiText = new GUIText(text, fontSize, m_fontType, position, viewportWidth, true, ScreenText);
	guiText->setInitialScale(1.0f);
	guiText->setScale(1.0f);
	TextEntity textEntity;
	textEntity.text = guiText;
	textEntity.lifeTime = lifetime;
	textEntity.currentLifeTime = 0.0f;
	textEntity.behaviour = behaviour;
	textEntity.velocity = velocity;
	m_textEntities.push_back(textEntity);

	return guiText;
}

