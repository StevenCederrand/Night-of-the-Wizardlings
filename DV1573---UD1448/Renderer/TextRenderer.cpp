#include "Pch/Pch.h"
#include "TextRenderer.h"

TextRenderer::TextRenderer()
{
	
}

TextRenderer::~TextRenderer()
{
	
}

void TextRenderer::cleanup()
{
	delete m_staticTextShader;
	m_textMap.clear();
}

void TextRenderer::init(Camera* camera)
{
	m_camera = camera;
	m_staticTextShader = new Shader("StaticTextShader.vs", "StaticTextShader.fs");
}

TextRenderer* TextRenderer::getInstance()
{
	static TextRenderer c;
	return &c;
}

void TextRenderer::submitText(GUIText* text)
{
	if (m_staticTextShader == nullptr) {
		logError("Please sir, initialize the text renderer if you're planning to use it :)");
		return;
	}

	auto it = m_textMap.find(text->getFontType());

	if (it != m_textMap.end()) {
		it._Ptr->_Myval.second.push_back(text);
	}
	else {
		std::vector<GUIText*> vec;
		vec.reserve(100);
		vec.push_back(text);
		m_textMap[text->getFontType()] = vec;

	}

}

void TextRenderer::removeText(GUIText* text)
{
	auto it = m_textMap.find(text->getFontType());

	if (it != m_textMap.end()) {

		auto vec = it._Ptr->_Myval.second;

		for (size_t i = 0; i < vec.size(); i++) {
			if (vec[i]->getUniqueIndex() == text->getUniqueIndex())
			{
				vec.erase(vec.begin() + i);
				return;
			}
		}

	}
}

void TextRenderer::renderText()
{
	//glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	for (const auto& item : m_textMap) {

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, item.first->getTextureAtlas());

		m_staticTextShader->use();

		for (auto* text : item.second) {
			
			if (text->shouldRender() == false)
				continue;


			if (text->isScreenText()) {
				renderScreenText(text);
			}
			else {
				renderWorldText(text);
			}

		}

		glBindTexture(GL_TEXTURE_2D, NULL);

	}


	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_textMap.clear();
}

void TextRenderer::renderWorldText(GUIText* text)
{
	text->updateModelMatrix();

	if (text->getFaceCamera())
		text->rotateTowardsCamera(m_camera);

	if (text->ignoreDepthTest()) {
		glDisable(GL_DEPTH_TEST);
	}
	else {
		glEnable(GL_DEPTH_TEST);
	}

	m_staticTextShader->setMat4("modelMatrix", text->getModelMatrix());
	m_staticTextShader->setMat4("viewMatrix", m_camera->getViewMat());
	m_staticTextShader->setMat4("projectionMatrix", m_camera->getProjMat());
	m_staticTextShader->setVec4("color", text->getColor());

	glBindVertexArray(text->getMeshVao());
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLES, 0, text->getVertexCount());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(NULL);
}

void TextRenderer::renderScreenText(GUIText* text)
{

	text->updateModelMatrix();
	m_staticTextShader->setMat4("modelMatrix", text->getModelMatrix());
	m_staticTextShader->setMat4("viewMatrix", glm::mat4(1.0f));
	m_staticTextShader->setMat4("projectionMatrix", glm::mat4(1.0f));
	m_staticTextShader->setVec4("color", text->getColor());

	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(text->getMeshVao());
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLES, 0, text->getVertexCount());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(NULL);
}
