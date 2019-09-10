#include <Pch/Pch.h>
#include "Input.h"


bool Input::m_keysPressed[MAX_KEYS];
bool Input::m_keysHeldDown[MAX_KEYS];
bool Input::m_keysReleased[MAX_KEYS];

bool Input::m_mousePressed[MAX_MOUSE_KEYS];
bool Input::m_mouseHeldDown[MAX_MOUSE_KEYS];
bool Input::m_mouseReleased[MAX_MOUSE_KEYS];

glm::vec2 Input::m_mousePosition;
bool Input::m_isWindowFocused = true;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Input* inputclass = reinterpret_cast<Input*>(glfwGetWindowUserPointer(window));
	inputclass->handleKeyInput(window, key, scancode, action, mods);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Input* inputclass = reinterpret_cast<Input*>(glfwGetWindowUserPointer(window));
	inputclass->handleMouseInput(window, button, action, mods);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Input* inputclass = reinterpret_cast<Input*>(glfwGetWindowUserPointer(window));
	inputclass->handleCursorMovement(window, xpos, ypos);
}

void window_focus_callback(GLFWwindow* window, int focused)
{
	Input* inputclass = reinterpret_cast<Input*>(glfwGetWindowUserPointer(window));
	inputclass->handleWindowFocused(window, focused);
}


Input::Input()
{
	glfwSetWindowUserPointer(glfwGetCurrentContext(), this);
	glfwSetKeyCallback(glfwGetCurrentContext(), key_callback);
	glfwSetMouseButtonCallback(glfwGetCurrentContext(), mouse_button_callback);
	glfwSetCursorPosCallback(glfwGetCurrentContext(), cursor_position_callback);
	glfwSetWindowFocusCallback(glfwGetCurrentContext(), window_focus_callback);
}

Input::~Input()
{
}

void Input::handleKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key >= MAX_KEYS || key < 0)
		return;

	if (action == GLFW_PRESS)
	{
		m_keysPressed[key] = true;
		m_keysHeldDown[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		m_keysReleased[key] = true;
		m_keysHeldDown[key] = false;
	}
}

void Input::handleMouseInput(GLFWwindow* window, int button, int action, int mods)
{
	if (button >= MAX_MOUSE_KEYS || button < 0)
		return;

	if (action == GLFW_PRESS)
	{
		m_mousePressed[button] = true;
		m_mouseHeldDown[button] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		m_mouseReleased[button] = true;
		m_mouseHeldDown[button] = false;
	}
}

void Input::handleCursorMovement(GLFWwindow* window, double xpos, double ypos)
{
	m_mousePosition.x = (float)xpos;
	m_mousePosition.y = (float)ypos;
}

void Input::handleWindowFocused(GLFWwindow* window, int focused)
{
	m_isWindowFocused = focused;
}

void Input::clearKeys()
{
	for (int i = 0; i < MAX_KEYS; i++)
	{
		m_keysPressed[i] = false;
		m_keysReleased[i] = false;

		if (i < MAX_MOUSE_KEYS)
		{
			m_mousePressed[i] = false;
			m_mouseReleased[i] = false;
		}
	}
}

bool Input::isKeyPressed(int keycode)
{
	if (keycode >= MAX_KEYS)
	{
		printf("Input Error (keyPressed): Keycode greater than max keys allowed\n");
		return false;
	}

	return m_keysPressed[keycode];
}
bool Input::isKeyReleased(int keycode)
{
	if (keycode >= MAX_KEYS)
	{
		printf("Input Error (keyReleased): Keycode greater than max keys allowed\n");
		return false;
	}

	return m_keysReleased[keycode];
}
bool Input::isKeyHeldDown(int keycode)
{
	if (keycode >= MAX_KEYS)
	{
		printf("Input Error (keyHeldDown): Keycode greater than max keys allowed\n");
		return false;
	}

	return m_keysHeldDown[keycode];
}
bool Input::isMousePressed(int button)
{
	if (button >= MAX_MOUSE_KEYS)
	{
		printf("Input Error (mousePressed): button greater than max buttons allowed\n");
		return false;
	}

	return m_mousePressed[button];
}
bool Input::isMouseHeldDown(int button)
{
	if (button >= MAX_MOUSE_KEYS)
	{
		printf("Input Error (mouseHeldDown): Keycode greater than max keys allowed\n");
		return false;
	}

	return m_mouseHeldDown[button];
}
bool Input::isMouseReleased(int button)
{
	if (button >= MAX_MOUSE_KEYS)
	{
		printf("Input Error (mouseReleased): Keycode greater than max keys allowed\n");
		return false;
	}

	return m_mouseReleased[button];

}
bool Input::isWindowFocused()
{
	return m_isWindowFocused;
}
const glm::vec2& Input::getMousePosition()
{
	return m_mousePosition;
}