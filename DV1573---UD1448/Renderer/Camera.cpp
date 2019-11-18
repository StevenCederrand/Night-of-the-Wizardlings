#include <Pch/Pch.h>
#include <System/Input.h>
#include "Camera.h"
#include <Networking/Client.h>

void Camera::freeCameraMode()
{
	updateMouseMovement();
	
	glm::vec3 moveDir = glm::vec3(0.0f);
	// Move
	if (Input::isKeyHeldDown(GLFW_KEY_A))
		moveDir -= m_camRight;
	if (Input::isKeyHeldDown(GLFW_KEY_D))
		moveDir += m_camRight;
	if (Input::isKeyHeldDown(GLFW_KEY_W))
		moveDir += m_camFace;
	if (Input::isKeyHeldDown(GLFW_KEY_S))
		moveDir -= m_camFace;

	m_camPos += moveDir * DeltaTime * m_spectatorMoveSpeed;

	m_viewMatrix = glm::lookAt(m_camPos, m_camPos + m_camFace, m_camUp);
}

void Camera::thirdPersonCamera()
{
}

void Camera::firstPersonCamera()
{
}

void Camera::calcVectors()
{
	
	m_camFace.x = cos(glm::radians(m_camYaw)) * cos(glm::radians(m_camPitch));
	m_camFace.y = sin(glm::radians(m_camPitch));
	m_camFace.z = sin(glm::radians(m_camYaw)) * cos(glm::radians(m_camPitch));
	m_camFace = glm::normalize(m_camFace);

	m_camRight = glm::normalize(glm::cross(m_camFace, m_worldUp));
	m_camUp = glm::normalize(glm::cross(m_camRight, m_camFace));
}

void Camera::resetCamera()
{

	int wSizeX, wSizeY;
	glfwGetWindowSize(glfwGetCurrentContext(), &wSizeX, &wSizeY);
	m_lastX = static_cast<float>(wSizeX / 2);
	m_lastY = static_cast<float>(wSizeY / 2);
	glfwSetCursorPos(glfwGetCurrentContext(), m_lastX, m_lastY);
	m_camYaw = -90.0f;
	m_camPitch = 0;
	m_camFace = glm::vec3(0.0f, 0.0f, -1.0f);
	calcVectors();
}

void Camera::updateMouseMovement()
{
	int wSizeX, wSizeY;
	glfwGetWindowSize(glfwGetCurrentContext(), &wSizeX, &wSizeY);
	m_lastX = static_cast<float>(wSizeX / 2);
	m_lastY = static_cast<float>(wSizeY / 2);

	glfwGetCursorPos(glfwGetCurrentContext(), &m_xpos, &m_ypos);
	glfwSetCursorPos(glfwGetCurrentContext(), m_lastX, m_lastY);
	if (this->m_firstMouse == true)
	{
		this->m_firstMouse = false;
		m_xpos = m_lastX;
		m_ypos = m_lastY;
	}

	float xoffset = static_cast<float>(m_xpos) - m_lastX;
	float yoffset = m_lastY - static_cast<float>(m_ypos);
	
	m_lastX = static_cast<float>(m_xpos);
	m_lastY = static_cast<float>(m_ypos);

	mouseControls(xoffset, yoffset, true);

	m_viewMatrix = glm::lookAt(m_camPos, m_camPos + m_camFace, m_camUp);
}

Camera::Camera()
{
	//Initial values (starting point of camera) if nothing else is given
	m_camPos = glm::vec3(0.0f, 3.0f, 0.0f);
	m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	m_camYaw = -90.0f;
	m_camPitch = 0;//-40.0f;
	
	m_camFace = glm::vec3(0.0f, 0.0f, -1.0f);
	m_camSpeed = 10;
	m_sensitivity = 0.15f;

	m_width = SCREEN_WIDTH;
	m_height = SCREEN_HEIGHT;

	m_nearPlane = 0.1f;
	m_farPlane = 200.0f;

	m_spectatorMoveSpeed = 20.0f;
	setWindowSize(m_width, m_height);
	calcVectors();
	m_viewMatrix = glm::lookAt(m_camPos, m_camPos + m_camFace, m_camUp);
	m_fpEnabled = true;
	m_activeCamera = true;

}

Camera::~Camera()
{

}

void Camera::fpsControls(const float& dt)
{
	float m_CamSpeed = m_camSpeed * dt;

	calcVectors();
}

void Camera::setWindowSize(float width, float height)
{
	this->m_width = width;
	this->m_height = height;
	
	setProjMat(this->m_width, this->m_height, this->m_nearPlane, this->m_farPlane);
}

void Camera::mouseControls(float xOffset, float yOffset, bool pitchLimit)
{
	xOffset *= m_sensitivity;
	yOffset *= m_sensitivity;
	
	m_camYaw += xOffset;
	m_camPitch += yOffset;

	if (pitchLimit)
	{
		if (m_camPitch > 89.0f)
			m_camPitch = 89.0f;
		if (m_camPitch < -89.0f)
			m_camPitch = -89.0f;
	}

	calcVectors();
}

void Camera::setProjMat(float widht, float height, float nearPlane, float farPlane)
{
	m_projectionMatrix = glm::perspective(glm::radians(60.0f), widht / height, nearPlane, farPlane);
}

const glm::mat4 Camera::getViewMat() const
{
	return m_viewMatrix;
}

const glm::mat4& Camera::getProjMat() const
{
	return m_projectionMatrix;
}

const double& Camera::getXpos() const
{
	return m_xpos;
}

const double& Camera::getYpos() const
{
	return m_ypos;
}

const float& Camera::getPitch() const
{
	return m_camPitch;
}

const float& Camera::getYaw() const
{
	return m_camYaw;
}

const glm::vec3& Camera::getCamPos() const
{
	return m_camPos;
}

const bool& Camera::isFPEnabled() const
{
	return m_fpEnabled;
}

const bool& Camera::isCameraActive() const
{
	return m_activeCamera;
}

const glm::vec3& Camera::getCamFace()
{
	return m_camFace;
}


void Camera::setCameraPos(const glm::vec3& pos)
{
	m_camPos = pos;
}

const glm::vec3& Camera::getCamRight()
{
	return m_camRight;
}

const SpectatorMode& Camera::getSpectatorMode() const
{
	return m_spectatorMode;
}

void Camera::lookAt(const glm::vec3& position)
{
	m_camFace = glm::normalize(position - m_camPos);
	m_viewMatrix = glm::lookAt(m_camPos, m_camPos + m_camFace, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::update()
{

	if (m_fpEnabled && m_activeCamera) {

		if (Client::getInstance()->isSpectating()) {

			if (m_spectatorMode == SpectatorMode::FreeCamera) {
				freeCameraMode();
			}
			else if (m_spectatorMode == SpectatorMode::ThirdPerson) {
				thirdPersonCamera();
			}
			else if (m_spectatorMode == SpectatorMode::FirstPerson) {
				firstPersonCamera();
			}

		}
		else {
			updateMouseMovement();
		
		}

	}
	m_viewMatrix = glm::lookAt(m_camPos, m_camPos + m_camFace, m_camUp);

}

void Camera::enableFP(const bool& fpEnable) {
	m_fpEnabled = fpEnable;
	//when enabling the fps camera
	if (m_fpEnabled) {
		int wSizeX, wSizeY;
		glfwGetWindowSize(glfwGetCurrentContext(), &wSizeX, &wSizeY);
		m_lastX = static_cast<float>(wSizeX / 2);
		m_lastY = static_cast<float>(wSizeY / 2);
		glfwSetCursorPos(glfwGetCurrentContext(), m_lastX, m_lastY);
	}
}

void Camera::disableCameraMovement(const bool condition)
{
	m_activeCamera = !condition;
}

void Camera::setSpectatorMode(SpectatorMode mode)
{
	m_spectatorMode = mode;
}
