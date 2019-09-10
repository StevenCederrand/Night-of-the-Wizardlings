#ifndef _INPUT_H
#define _INPUT_H
#include <Pch/Pch.h>

/* 
	This is a static class and is initialized in application.
	To use this just include the "Input.h" header in the CPP class that
	you want to use it in and then for example you could do:

	Input::isKeyPressed(GLFW_KEY_W)

	which returns if the W key was pressed or not during that frame.
*/

class Application;

#define MAX_KEYS 375
#define MAX_MOUSE_KEYS 8

class Input
{
public:
	Input();
	~Input();

	static bool isKeyPressed(int keycode);
	static bool isKeyReleased(int keycode);
	static bool isKeyHeldDown(int keycode);
	static bool isMousePressed(int button);
	static bool isMouseHeldDown(int button);
	static bool isMouseReleased(int button);
	static bool isWindowFocused();
	static const glm::vec2& getMousePosition();

private:

	friend class Application;
	friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	friend void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	friend void window_focus_callback(GLFWwindow* window, int focused);

	void handleKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
	void handleMouseInput(GLFWwindow* window, int button, int action, int mods);
	void handleCursorMovement(GLFWwindow* window, double xpos, double ypos);
	void handleWindowFocused(GLFWwindow* window, int focused);

	void clearKeys();

	static bool m_keysPressed[MAX_KEYS];
	static bool m_keysHeldDown[MAX_KEYS];
	static bool m_keysReleased[MAX_KEYS];

	static bool m_mousePressed[MAX_MOUSE_KEYS];
	static bool m_mouseHeldDown[MAX_MOUSE_KEYS];
	static bool m_mouseReleased[MAX_MOUSE_KEYS];

	static glm::vec2 m_mousePosition;
	static bool m_isWindowFocused;
};

#endif