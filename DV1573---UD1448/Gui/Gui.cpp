#include <Pch/Pch.h>
#include "Gui.h"

CEGUI::OpenGL3Renderer* Gui:: m_guiRenderer = nullptr;

unsigned int GlfwToCeguiKey(int glfwKey)
{
	switch (glfwKey)
	{
	case GLFW_KEY_UNKNOWN: return 0;
	case GLFW_KEY_ESCAPE: return CEGUI::Key::Escape;
	case GLFW_KEY_F1: return CEGUI::Key::F1;
	case GLFW_KEY_F2: return CEGUI::Key::F2;
	case GLFW_KEY_F3: return CEGUI::Key::F3;
	case GLFW_KEY_F4: return CEGUI::Key::F4;
	case GLFW_KEY_F5: return CEGUI::Key::F5;
	case GLFW_KEY_F6: return CEGUI::Key::F6;
	case GLFW_KEY_F7: return CEGUI::Key::F7;
	case GLFW_KEY_F8: return CEGUI::Key::F8;
	case GLFW_KEY_F9: return CEGUI::Key::F9;
	case GLFW_KEY_F10: return CEGUI::Key::F10;
	case GLFW_KEY_F11: return CEGUI::Key::F11;
	case GLFW_KEY_F12: return CEGUI::Key::F12;
	case GLFW_KEY_F13: return CEGUI::Key::F13;
	case GLFW_KEY_F14: return CEGUI::Key::F14;
	case GLFW_KEY_F15: return CEGUI::Key::F15;
	case GLFW_KEY_UP: return CEGUI::Key::ArrowUp;
	case GLFW_KEY_DOWN: return CEGUI::Key::ArrowDown;
	case GLFW_KEY_LEFT: return CEGUI::Key::ArrowLeft;
	case GLFW_KEY_RIGHT: return CEGUI::Key::ArrowRight;
	case GLFW_KEY_TAB: return CEGUI::Key::Tab;
	case GLFW_KEY_ENTER: return CEGUI::Key::Return;
	case GLFW_KEY_BACKSPACE: return CEGUI::Key::Backspace;
	case GLFW_KEY_INSERT: return CEGUI::Key::Insert;
	case GLFW_KEY_HOME: return CEGUI::Key::Home;
	case GLFW_KEY_END: return CEGUI::Key::End;
	case GLFW_KEY_KP_ENTER: return CEGUI::Key::NumpadEnter;
	default: return 0;
	}
}

CEGUI::MouseButton GlfwToCeguiButton(int glfwButton)
{
	switch (glfwButton)
	{
	case GLFW_MOUSE_BUTTON_LEFT: return CEGUI::LeftButton;
	case GLFW_MOUSE_BUTTON_RIGHT: return CEGUI::RightButton;
	case GLFW_MOUSE_BUTTON_MIDDLE: return CEGUI::MiddleButton;
	default: return CEGUI::NoButton;
	}
}

void Gui::init()
{
	if (m_guiRenderer == nullptr)
	{
		m_guiRenderer = &CEGUI::OpenGL3Renderer::bootstrapSystem();
		CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
		rp->setResourceGroupDirectory("imagesets",	 "DV1573---UD1448/GUIResource/imagesets/");
		rp->setResourceGroupDirectory("fonts",		 "DV1573---UD1448/GUIResource/fonts/");
		rp->setResourceGroupDirectory("layouts",	 "DV1573---UD1448/GUIResource/layouts/");
		rp->setResourceGroupDirectory("looknfeel",	 "DV1573---UD1448/GUIResource/looknfeel/");
		rp->setResourceGroupDirectory("lua_scripts", "DV1573---UD1448/GUIResource/lua_scripts/");
		rp->setResourceGroupDirectory("schemes",	 "DV1573---UD1448/GUIResource/schemes/");
		//rp->setResourceGroupDirectory("xml_schemes", "GUIResource/xml_schemes/");
		//rp->setResourceGroupDirectory("animations",  "GUIResource/animations/");

		CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
		CEGUI::Scheme::setDefaultResourceGroup("schemes");
		CEGUI::Font::setDefaultResourceGroup("fonts");
		CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeel");
		CEGUI::WindowManager::setDefaultResourceGroup("layout");
		CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
	}

	m_context = &CEGUI::System::getSingleton().createGUIContext(m_guiRenderer->getDefaultRenderTarget());
	m_root = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "root");
	m_context->setRootWindow(m_root);
}

void Gui::destroy()
{
	CEGUI::System::getSingleton().destroyGUIContext(*m_context);
}

void Gui::loadScheme(const std::string& schemefile)
{
	CEGUI::SchemeManager::getSingleton().createFromFile(schemefile);
}

void Gui::setFont(const std::string& fontFile)
{
	CEGUI::FontManager::getSingleton().createFromFile(fontFile + ".font");
	m_context->setDefaultFont(fontFile);
}

CEGUI::Window* Gui::createWidget(const std::string& scheme, const glm::vec4& destRecPerc, const glm::vec4& destRecPix, const std::string& name)
{
	CEGUI::Window* newWindow = CEGUI::WindowManager::getSingleton().createWindow(scheme, name);
	m_root->addChild(newWindow);
	setWidgetDestRect(newWindow, destRecPerc, destRecPix);
	return newWindow;


}

void Gui::setWidgetDestRect(CEGUI::Window* widget, const glm::vec4& destRecPerc, const glm::vec4& destRecPix)
{
	widget->setPosition(CEGUI::UVector2(CEGUI::UDim(destRecPerc.x, destRecPix.x), CEGUI::UDim(destRecPerc.y, destRecPix.y)));
	widget->setSize(CEGUI::USize(CEGUI::UDim(destRecPerc.z, destRecPix.z), CEGUI::UDim(destRecPerc.w, destRecPix.w)));
}

void Gui::setMouseCursor(const std::string& imageFile)
{
	m_context->getMouseCursor().setDefaultImage(imageFile);
}

void Gui::showMouseCursor()
{
	m_context->getMouseCursor().show();
}

void Gui::hideMouseCursor()
{
	m_context->getMouseCursor().hide();
}

void Gui::update(float dt)
{
	m_context->injectTimePulse(dt);
	CEGUI::utf32 codePoint;
	if (m_context->getMouseCursor().isVisible())
	{
		double x, y;
		glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);
		m_context->injectMousePosition(x,y);
	}

	if (Input::isAnyKeyPressed())
	{
		CEGUI::Key::Scan code = (CEGUI::Key::Scan)GlfwToCeguiKey(Input::getLatestKeyPressed());
		m_context->injectKeyDown(code);
	}
	if (Input::isAnyKeyReleased())
	{
		CEGUI::Key::Scan code = (CEGUI::Key::Scan)GlfwToCeguiKey(Input::getLatestKeyPressed());
		m_context->injectKeyUp(code);
	}  
	if (Input::isTextInput())
	{
		m_context->injectChar((CEGUI::utf32)Input::getLatestPressedCharacter());
	}
	if (Input::isAnyMousePressed())
	{
		m_context->injectMouseButtonDown(GlfwToCeguiButton(Input::getLatestPressedMouseButton()));
	} 
	if (Input::isAnyMouseReleased())
	{
		m_context->injectMouseButtonUp(GlfwToCeguiButton(Input::getLatestPressedMouseButton()));
	}
	
}

void Gui::draw()
{
	glDisable(GL_DEPTH_TEST);
	m_guiRenderer->beginRendering();
	m_context->draw();
	m_guiRenderer->endRendering();
	glDisable(GL_SCISSOR_TEST);
	glEnable(GL_DEPTH_TEST);
}
