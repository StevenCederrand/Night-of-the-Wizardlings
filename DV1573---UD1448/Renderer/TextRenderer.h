#ifndef _TEXT_RENDERER_H
#define _TEXT_RENDERER_H
#include <Pch/Pch.h>
#include "Camera.h"
#include "ShaderMap.h"
#include <BetterText/FontType.h>
#include <BetterText/GUIText.h>

class TextRenderer {

public:
	TextRenderer();
	~TextRenderer();
	void cleanup();
	void init(Camera* camera);

	static TextRenderer* getInstance();

	void submitText(GUIText* text);
	void removeText(GUIText* text);
	
	void renderText();


private:
	std::map <FontType*, std::vector<GUIText*>> m_textMap;
	Shader* m_staticTextShader;
	Camera* m_camera;
};


#endif
