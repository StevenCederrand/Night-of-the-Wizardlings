#ifndef _GAMEGUI_h
#define _GAMEGUI_h
#include <Pch/Pch.h>
#include <Gui/Gui.h>


class GameGUI {
public:
	GameGUI();
	~GameGUI();

	void loadGUI();
	void unloadGUI();

private:
	CEGUI::PushButton* m_mainMenu;
	CEGUI::PushButton* m_quit;

};

#endif