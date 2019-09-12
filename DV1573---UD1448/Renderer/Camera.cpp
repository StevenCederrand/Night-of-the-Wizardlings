#include <Pch/Pch.h>
#include <System/Input.h>
#include "Camera.h"

void Camera::calcVectors()
{
	glm::vec3 face;
	face.x = cos(glm::radians(camYaw)) * cos(glm::radians(camPitch));
	face.y = sin(glm::radians(camPitch));
	face.z = sin(glm::radians(camYaw)) * cos(glm::radians(camPitch));

	camFace = glm::normalize(face);

	camRight = glm::normalize(glm::cross(camFace, worldUp));
	camUp = glm::normalize(glm::cross(camRight, camFace));
}

Camera::Camera()
{
	//Initial values (starting point of camera) if nothing else is given
	camPos = glm::vec3(0.0f, 0.0f, 0.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	camYaw = 90.0f;
	camPitch = -40.0f;
	
	camFace = glm::vec3(0.0f, 0.0f, 1.0f);
	camSpeed = 3.5f;
	sensitivity = 0.15f;

	width = SCREEN_WIDTH;
	height = SCREEN_HEIGHT;

	nearPlane = 0.1f;
	farPlane = 200.0f;


	calcVectors();
}

Camera::~Camera()
{
}

void Camera::fpsControls(GLFWwindow* window, float deltaTime)
{
	float m_CamSpeed = camSpeed * deltaTime;

	//WASD controls
	if (Input::isKeyPressed(GLFW_KEY_A))
		camPos -= m_CamSpeed * camRight;
	if (Input::isKeyPressed(GLFW_KEY_D))
		camPos += m_CamSpeed * camRight;
	if (Input::isKeyPressed(GLFW_KEY_W))
		camPos += m_CamSpeed * camFace;
	if (Input::isKeyPressed(GLFW_KEY_S))
		camPos -= m_CamSpeed * camFace;

	calcVectors();
}

void Camera::setWindowSize(float width, float height)
{
	this->width = width;
	this->height = height;
	
	setProjMat(this->width, this->height, this->nearPlane, this->farPlane);
}

void Camera::mouseControls(float xOffset, float yOffset, bool pitchLimit)
{
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	camYaw += xOffset;
	camPitch += yOffset;

	if (pitchLimit)
	{
		if (camPitch > 89.0f)
			camPitch = 89.0f;
		if (camPitch < -89.0f)
			camPitch = 89.0f;
	}

	calcVectors();
}

void Camera::setProjMat(float widht, float height, float nearPlane, float farPlane)
{
	projMat = glm::perspective((3.14f * 0.45f), widht / height, nearPlane, farPlane);
}

glm::mat4 Camera::getViewMat() const
{
	return glm::lookAt(camPos, camPos + camFace, camUp);
}

glm::mat4 Camera::getProjMat() const
{
	return projMat;
}
