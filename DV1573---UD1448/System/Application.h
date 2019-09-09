#ifndef _APPLICATION_h
#define _APPLICATION_h
#include <GL/glew.h>
#include "Input.h"
#include "StateManager.h"

class Application {

public:
	Application();
	~Application();
	
	/* Init GLFW & GL HEHEH */
	bool init();
	
	/* Runs the application */
	void run();


private:
	GLFWwindow* m_window;
	Input* m_input;
	StateManager* m_stateManager;
	PersistentData m_pd;
	bool m_vsync;

};




#endif
