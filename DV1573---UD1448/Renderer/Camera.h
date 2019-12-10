#pragma once
#include <Pch/Pch.h>

enum SpectatorMode {
	FreeCamera,
	FirstPerson,
	ThirdPerson
};

class Camera
{
private:
	// Spectator Modes
	void freeCameraMode();
	void thirdPersonCamera();
	void firstPersonCamera();

	// Level Edit modes
	void LE_freeCamera();
	void LE_orbitCamera();

	void lookForModeChange();
	void resetMouseToMiddle();

	float m_spectatorMoveSpeed;

	void calcVectors();
	const PlayerPacket* m_spectatedPlayer;
	SpectatorMode m_spectatorMode;
	bool m_firstMouse = true;
	double m_xpos, m_ypos;
	float m_lastX, m_lastY;

	glm::vec3 m_camPos;
	glm::vec3 m_camFace;
	glm::vec3 m_worldUp;
	glm::vec3 m_camUp;
	glm::vec3 m_camRight;

	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;

	float m_camYaw;
	float m_camPitch;
	float m_width;
	float m_height;
	float m_nearPlane;
	float m_farPlane;
	float m_camSpeed;

	void updateMouseMovement();
	void updateThirdPersonMouseMovement();
	bool m_fpEnabled;
	bool m_activeCamera;
	int m_cameraControlSwitch;
	glm::vec2 oldPosition;
public:
	Camera();
	Camera(glm::vec3 pos, float yaw, float pitch);
	~Camera();

	void fpsControls(const float& dt);
	void setWindowSize(float width, float height);
	void mouseControls(float xOffset, float yOffset, bool pitchLimit);
	void setProjMat(float width, float height, float nearPlane, float farPlane);
	void setCameraPos(const glm::vec3& pos);
	void lookAt(const glm::vec3& position);
	void enableFP(const bool& fpEnable);
	void disableCameraMovement(const bool condition);
	void setSpectatorMode(SpectatorMode mode);
	void resetCamera();

	void spectatePlayer(const PlayerPacket* playerPacket);

	const glm::mat4 getViewMat() const;
	const glm::mat4& getProjMat() const;
	const double& getXpos() const;
	const double& getYpos() const;

	const float& getPitch() const;
	const float& getYaw() const;
	const glm::vec3& getCamPos() const;
	const bool& isFPEnabled() const;
	const bool& isCameraActive() const;

	const glm::vec3& getCamFace();
	const glm::vec3& getCamRight();
	const glm::vec3& getCamUp();	
	const SpectatorMode& getSpectatorMode() const;

	void update();
	void updateLevelEd();
	
};

