#ifndef _APPLICATION_h
#define _APPLICATION_h
#include <Pch/Pch.h>
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
	void initGraphics();
	
	
	GLFWwindow* m_window;
	Camera* m_camera;
	Input* m_input;
	StateManager* m_stateManager;
	bool m_vsync;
	


	Renderer* m_renderer;
	ShaderMap* m_shaderMap;

};




#endif
