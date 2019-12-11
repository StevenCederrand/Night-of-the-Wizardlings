#include <Pch/Pch.h>
#include "MousePicker.h"

MousePicker::MousePicker(Camera* camera, glm::mat4 projMatrix)
{
	m_camera = camera;
	this->projMatrix = projMatrix;
	viewMatrix = m_camera->getViewMat();
}

MousePicker::~MousePicker()
{
}


void MousePicker::update()
{
	viewMatrix = m_camera->getViewMat();
	currentRay = calculateMouseRay();
}

glm::vec3 MousePicker::calculateMouseRay()
{
	float mouseX = Input::getMousePosition().x;
	float mouseY = Input::getMousePosition().y;
	glm::vec2 normalizedCoords = getNormalizedDeviceCoords(mouseX, mouseY);
	glm::vec4 clipCoords = glm::vec4(normalizedCoords.x, normalizedCoords.y, -1.f, 1.f);
	glm::vec4 eyeCoords = toEyeCoords(clipCoords);
	glm::vec3 worldRay = toWorldCoords(eyeCoords);
	return worldRay;
}

glm::vec2 MousePicker::getNormalizedDeviceCoords(float mouseX, float mouseY)
{
	float x = (2.0f * mouseX) / SCREEN_WIDTH - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / SCREEN_HEIGHT;
	return glm::vec2(x, y);
}

glm::vec4 MousePicker::toEyeCoords(glm::vec4 clipCoords)
{
	glm::vec4 eyeCoords = inverse(projMatrix) * clipCoords;
	return glm::vec4(eyeCoords.x, eyeCoords.y, -1.f, 0.f);
}

glm::vec3 MousePicker::toWorldCoords(glm::vec4 eyeCoords)
{	
	glm::vec3 worldRay = (inverse(viewMatrix) * eyeCoords);
	worldRay = glm::normalize(worldRay);
	return worldRay;
}
