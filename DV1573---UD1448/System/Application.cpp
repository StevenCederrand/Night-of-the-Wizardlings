#include "Application.h"
#include <GLFW/glfw3.h>
Application::Application() {
}

Application::~Application() {

}

bool Application::init() {

	bool x = glfwInit();



	return x;
}

void Application::update()
{
}
