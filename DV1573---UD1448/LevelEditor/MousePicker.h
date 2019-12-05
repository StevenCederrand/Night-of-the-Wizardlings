#ifndef _MOUSE_PICKER_H
#define _MOUSE_PICKER_H
#include <Pch/Pch.h>

class MousePicker
{
public:
	MousePicker(Camera* camera, glm::mat4 projMatrix);
	~MousePicker();

	glm::vec3 getCurrentRay() { return currentRay; }
	void update();

private:
	glm::vec3 currentRay;
	glm::mat4 projMatrix;
	glm::mat4 viewMatrix;

	Camera* m_camera;

private:
	glm::vec3 calculateMouseRay();
	glm::vec3 getNormalizedDeviceCoords(float mouseX, float mouseY);
	glm::vec4 toEyeCoords(glm::vec4 clipCoords);
	glm::vec3 toWorldCoords(glm::vec4 eyeCoords);
};


#endif