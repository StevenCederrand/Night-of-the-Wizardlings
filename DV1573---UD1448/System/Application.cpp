#include <Pch/Pch.h>
#include "Application.h"
#include "States/PlayState.h"

Application::Application() {
}

Application::~Application() {
	delete m_input;
	delete m_stateManager;
	glfwTerminate();
	ShaderMap::cleanUp();
}

bool Application::init() {

	bool statusOK = false;

	statusOK = glfwInit();

	if (!statusOK) {
		logError("Failed to initialize GLFW!");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	m_window = glfwCreateWindow(1280, 720, "Wizards 'n stuff", NULL, NULL);

	if (m_window == nullptr) {
		glfwTerminate();
		logError("Failed to create GLFW window");
		return false;
	}

	// Opengl context
	glfwMakeContextCurrent(m_window);

	GLenum status = glewInit();

	if (status != GLEW_OK) {
		glfwTerminate();
		logError("Failed to initialize GLEW!");
	}

	// Vsync
	m_vsync = true;
	glfwSwapInterval(1);
	
	m_input = new Input();

	/*
		Initialize all persisten data here
	*/
	m_pd.Renderer = Renderer(m_camera, m_window);
	
	m_stateManager = new StateManager(&m_pd);

	m_stateManager->pushState(new PlayState());

	logTrace("Application successfully initialized");
	return statusOK;
}

void Application::run()
{
	float timeNow = 0.0f;
	float timeThen = 0.0f;

	logInfo("Running Application loop");

	while (!glfwWindowShouldClose(m_window)) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		m_input->clearKeys();
		glfwPollEvents();

		// Quick way to close the app
		if (Input::isKeyReleased(GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(m_window, true);
		}
		/*
		if (Input::isKeyPressed(GLFW_KEY_R)) {

			//glfwMakeContextCurrent(NULL);
			ShaderMap::reload();
			glfwMakeContextCurrent(m_window);
		}*/
		//Deltatime
		float deltaTime = timeNow - timeThen;
		timeThen = timeNow;


		m_stateManager->update(deltaTime);
		m_stateManager->render();	

		glfwSwapBuffers(m_window);
	}

	logInfo("Exiting application loop");

}
