#ifndef _GUI_H
#define _GUI_H
#include <Pch/Pch.h>

class Gui {

public:
	// Only needs to be called once in the application
	// Doesn't really matter if you call it twice but 
	// you dont have to do it.
	void init();
	
	void destroy();


	void loadScheme(const std::string& schemefile);
	void setFont(const std::string& fontFile);
	CEGUI::Window* createWidget(const std::string& section, const std::string& scheme, const glm::vec4& destRecPerc, const glm::vec4& destRecPix, const std::string& name);
	static void setWidgetDestRect(CEGUI::Window* widget, const glm::vec4& destRecPerc, const glm::vec4& destRecPix);
	static Gui* getInstance();
	void setMouseCursor(const std::string& imageFile);
	void showMouseCursor();
	void hideMouseCursor();
	void update(float dt);
	void draw();
	void clearWidgetsInSection(const std::string& section);

	// Getters
	static CEGUI::OpenGL3Renderer* getRenderer() { return m_guiRenderer; }
	const CEGUI::GUIContext* getContext() { return m_context; }

private:
	static CEGUI::OpenGL3Renderer* m_guiRenderer;
	CEGUI::GUIContext* m_context = nullptr;
	CEGUI::Window* m_root;
	
	std::unordered_map<std::string, std::vector<CEGUI::Window*>> m_widgetMap;
	

};


#endif
