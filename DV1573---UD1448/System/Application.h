#ifndef _APPLICATION_h
#define _APPLICATION_h
#include <Pch/Pch.h>
#include "Input.h"
#include "StateManager.h"
#include <Gui/NoLogger.h>

class Application {

public:
	Application();
	~Application();
	
	bool init();
	void run();


private:
	void initGraphics();
	void initSound();
	void calcFPS(const float& dt);
	void centerWindowOnMonitor();
	GLFWwindow* m_window;
	Input* m_input;
	StateManager* m_stateManager;	
	bool initialFrame;
	NoLogger* m_noLog;
};




#endif
