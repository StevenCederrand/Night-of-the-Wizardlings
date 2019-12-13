#include <Pch/Pch.h>
#include <System/Input.h>
#include "Camera.h"
#include <Networking/Client.h>

static float m_sensitivity;
static float m_distanceThirdPerson = 10.0f;
static float m_distanceModel = 100.f;
static glm::vec3 m_aim = glm::vec3(0.0f, 0.0f, 0.0f);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

	m_distanceThirdPerson -= yoffset;
	m_distanceModel -= yoffset*4;

	if (m_distanceThirdPerson <= 2.0f)
		m_distanceThirdPerson = 2.0f;
	else if (m_distanceThirdPerson >= 35.0f)
		m_distanceThirdPerson = 35.0f;
}

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
	if (Input::isKeyHeldDown(GLFW_KEY_R))
		moveDir += m_worldUp;
	if (Input::isKeyHeldDown(GLFW_KEY_F))
		moveDir -= m_worldUp;

	m_camPos += moveDir * DeltaTime * m_spectatorMoveSpeed;

	m_viewMatrix = glm::lookAt(m_camPos, m_camPos + m_camFace, m_camUp);
}

void Camera::thirdPersonCamera()
{
	updateThirdPersonMouseMovement();

	if (Input::isMousePressed(GLFW_MOUSE_BUTTON_LEFT)) {
		Client::getInstance()->spectateNext();
	}

	m_spectatedPlayer = Client::getInstance()->getSpectatedPlayer();
	
	if (m_spectatedPlayer == nullptr) {
		m_spectatorMode = SpectatorMode::FreeCamera;
		return;
	}

	const glm::vec3& playerpos = m_spectatedPlayer->position;
	const glm::vec3& meshHalfSize = m_spectatedPlayer->meshHalfSize;


	m_camPos.x = playerpos.x + (m_distanceThirdPerson * cos(glm::radians(m_camYaw)) * cos(glm::radians(m_camPitch)));
	m_camPos.y = meshHalfSize.y + playerpos.y + (m_distanceThirdPerson * sin(glm::radians(m_camPitch)));
	m_camPos.z = playerpos.z + (m_distanceThirdPerson * sin(glm::radians(m_camYaw)) * cos(glm::radians(m_camPitch)));

	lookAt(playerpos + glm::vec3(0.0f, meshHalfSize.y * 1.75f, 0.0f));

	
}

void Camera::firstPersonCamera()
{
}

void Camera::LE_freeCamera()
{
	glm::vec3 moveDir = glm::vec3(0.0f);
	//When alt is held down camera is controllable
	if (Input::isMouseHeldDown(GLFW_MOUSE_BUTTON_RIGHT))
	{
		updateMouseMovement();
		
		// Move
		if (Input::isKeyHeldDown(GLFW_KEY_A))
			moveDir -= m_camRight;
		if (Input::isKeyHeldDown(GLFW_KEY_D))
			moveDir += m_camRight;
		if (Input::isKeyHeldDown(GLFW_KEY_W))
			moveDir += m_camFace;
		if (Input::isKeyHeldDown(GLFW_KEY_S))
			moveDir -= m_camFace;
		if (Input::isKeyHeldDown(GLFW_KEY_R))
			moveDir += m_worldUp;
		if (Input::isKeyHeldDown(GLFW_KEY_F))
			moveDir -= m_worldUp;

		m_camPos += moveDir * DeltaTime * m_spectatorMoveSpeed;

		m_viewMatrix = glm::lookAt(m_camPos, m_camPos + m_camFace, m_camUp);
	}
}

void Camera::LE_orbitCamera()
{
	updateLEMouseMovement();
	m_camPos.x = m_aim.x + (m_distanceModel * cos(glm::radians(m_camYaw)) * cos(glm::radians(m_camPitch)));
	m_camPos.y = m_aim.y + (m_distanceModel * sin(glm::radians(m_camPitch)));
	m_camPos.z = m_aim.z + (m_distanceModel * sin(glm::radians(m_camYaw)) * cos(glm::radians(m_camPitch)));
	lookAt(glm::vec3(m_aim));
}

void Camera::cameraPan(float dx, float dy)
{
	m_panX += dx;
	m_panY += dy;
}

void Camera::lookForModeChange()
{
	if (Input::isKeyPressed(GLFW_KEY_E)) {
	
		if (m_spectatorMode == SpectatorMode::FreeCamera)
		{
			// Before changing look the current player that is spectated
			// see if it's null and if it is then go to next

			if (Client::getInstance()->getSpectatedPlayer() == nullptr)
				Client::getInstance()->spectateNext();

			resetMouseToMiddle();
			m_camPitch *= -1.0f;
			m_camYaw -= 180.0f;
			m_spectatorMode = SpectatorMode::ThirdPerson;

		}else if (m_spectatorMode == SpectatorMode::ThirdPerson)
		{
			resetMouseToMiddle();
			m_camPitch *= -1.0f;
			m_camYaw -= 180.0f;
			calcVectors();
			//resetCamera();
			m_spectatorMode = SpectatorMode::FreeCamera;
		}

	}

}

void Camera::resetMouseToMiddle()
{
	int wSizeX, wSizeY;
	glfwGetWindowSize(glfwGetCurrentContext(), &wSizeX, &wSizeY);
	m_lastX = static_cast<float>(wSizeX / 2);
	m_lastY = static_cast<float>(wSizeY / 2);
	glfwSetCursorPos(glfwGetCurrentContext(), m_lastX, m_lastY);
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
	resetMouseToMiddle();
	m_camYaw = -90.0f;
	m_camPitch = 0;
	m_camFace = glm::vec3(0.0f, m_camPos.y, 0.0f);
	calcVectors();
}

void Camera::spectatePlayer(const PlayerPacket* playerPacket)
{
	m_spectatedPlayer = playerPacket;
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

void Camera::updateThirdPersonMouseMovement()
{
	static glm::dvec2 initialPos = glm::dvec2(0.0);
	
	if (Input::isMousePressed(GLFW_MOUSE_BUTTON_RIGHT)) {
		glfwGetCursorPos(glfwGetCurrentContext(), &initialPos.x, &initialPos.y);

	}

	if (Input::isMouseHeldDown(GLFW_MOUSE_BUTTON_RIGHT))
	{
		glm::dvec2 currentMouse = glm::dvec2(0.0);
		glfwGetCursorPos(glfwGetCurrentContext(), &currentMouse.x, &currentMouse.y);

		float xoffset = static_cast<float>(currentMouse.x) - initialPos.x;
		float yoffset = initialPos.y - static_cast<float>(currentMouse.y);

		xoffset *= m_sensitivity;
		yoffset *= m_sensitivity;

		m_camYaw += xoffset;
		m_camPitch -= yoffset;

		if (m_camPitch > 89.0f)
			m_camPitch = 89.0f;
		if (m_camPitch < -89.0f)
			m_camPitch = -89.0f;

		initialPos = currentMouse;
	}

}

void Camera::updateLEMouseMovement()
{
	static glm::dvec2 initPos = glm::dvec2(0.0);
	calcVectors();

	if (Input::isMousePressed(GLFW_MOUSE_BUTTON_RIGHT))
		glfwGetCursorPos(glfwGetCurrentContext(), &initPos.x, &initPos.y);
	
	if (Input::isMousePressed(GLFW_MOUSE_BUTTON_MIDDLE))
	{
		logTrace("LeftClick");

		glfwGetCursorPos(glfwGetCurrentContext(), &initPos.x, &initPos.y);
	}

	if (Input::isMouseHeldDown(GLFW_MOUSE_BUTTON_MIDDLE))
	{
		logTrace("LeftButton Held Down");
		glm::dvec2 currentMouse = glm::dvec2(0.0);
		glfwGetCursorPos(glfwGetCurrentContext(), &currentMouse.x, &currentMouse.y);

		float xOffset = static_cast<float>(currentMouse.x) - initPos.x;
		float yOffset = initPos.y - static_cast<float>(currentMouse.y);

		xOffset *= m_sensitivity;
		yOffset *= m_sensitivity;

		/*m_camRight += xOffset;
		m_camUp += yOffset;*/

		m_aim += m_camRight * xOffset - m_camUp * yOffset; 

		

		initPos = currentMouse;
	}
	if (Input::isKeyPressed(GLFW_KEY_F))
	{
		//This can and should be expanded upon to focus on selected target
		m_aim = glm::vec3(0.0f);
		m_distanceModel = 100.f;
	}

	if (Input::isMouseHeldDown(GLFW_MOUSE_BUTTON_RIGHT))
	{
		glm::dvec2 currentMouse = glm::dvec2(0.0);
		glfwGetCursorPos(glfwGetCurrentContext(), &currentMouse.x, &currentMouse.y);

		float xoffset = static_cast<float>(currentMouse.x) - initPos.x;
		float yoffset = initPos.y - static_cast<float>(currentMouse.y);

		xoffset *= m_sensitivity;
		yoffset *= m_sensitivity;

		m_camYaw += xoffset;
		m_camPitch -= yoffset;

		if (m_camPitch > 89.0f)
			m_camPitch = 89.0f;
		if (m_camPitch < -89.0f)
			m_camPitch = -89.0f;

		initPos = currentMouse;
	}
}



Camera::Camera()
{
	//Initial values (starting point of camera) if nothing else is given
	m_cameraControlSwitch = 1;

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
	m_farPlane = 1500.0f;

	m_spectatorMoveSpeed = 20.0f;
	setWindowSize(m_width, m_height);
	calcVectors();
	m_viewMatrix = glm::lookAt(m_camPos, m_camPos + m_camFace, m_camUp);
	m_fpEnabled = true;
	m_activeCamera = true;

	glfwSetScrollCallback(glfwGetCurrentContext(), scroll_callback);

}

Camera::Camera(glm::vec3 pos, float yaw, float pitch)
{
	//Initial values (starting point of camera) if nothing else is given
	m_camPos = pos;
	m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	m_camYaw = yaw;
	m_camPitch = pitch;//-40.0f;

	m_camFace = glm::vec3(0.0f, 0.0f, -1.0f);
	m_camSpeed = 10;
	m_sensitivity = 0.15f;

	m_width = SCREEN_WIDTH;
	m_height = SCREEN_HEIGHT;

	m_nearPlane = 0.1f;
	m_farPlane = 1500.0f;

	m_spectatorMoveSpeed = 20.0f;
	setWindowSize(m_width, m_height);
	calcVectors();
	m_viewMatrix = glm::lookAt(m_camPos, m_camPos + m_camFace, m_camUp);
	m_fpEnabled = true;
	m_activeCamera = true;

	glfwSetScrollCallback(glfwGetCurrentContext(), scroll_callback);

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


const glm::vec3& Camera::getCamUp()
{
	return m_camUp;
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

			lookForModeChange();

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
	if(!Client::getInstance()->isSpectating())
		m_viewMatrix = glm::lookAt(m_camPos, m_camPos + m_camFace, m_camUp);

}

void Camera::updateLevelEd()
{
	LE_orbitCamera();
}

void Camera::enableFP(const bool& fpEnable) {
	m_fpEnabled = fpEnable;
	//when enabling the fps camera
	if (m_fpEnabled) {
		resetMouseToMiddle();
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
