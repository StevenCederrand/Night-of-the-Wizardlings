#include <Pch/Pch.h>
#include "Gui.h"

CEGUI::OpenGL3Renderer* Gui:: m_guiRenderer = nullptr;

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

void Gui::draw()
{
	glDisable(GL_DEPTH_TEST);
	m_guiRenderer->beginRendering();
	m_context->draw();
	m_guiRenderer->endRendering();
	glDisable(GL_SCISSOR_TEST);
	glEnable(GL_DEPTH_TEST);
}
