#pragma once
#include <Pch/Pch.h>



class Camera
{
private:
	void calcVectors();
	
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
	float m_sensitivity;

	void updateMouseMovement();
	bool m_fpEnabled;
	bool m_activeCamera;
	glm::vec2 oldPosition;
public:
	Camera();
	~Camera();

	void fpsControls(const float& dt);
	void setWindowSize(float width, float height);
	void mouseControls(float xOffset, float yOffset, bool pitchLimit);
	void setProjMat(float width, float height, float nearPlane, float farPlane);
	void setCameraPos(const glm::vec3& pos);
	void lookAt(const glm::vec3& position);
	void enableFP(const bool& fpEnable);
	void disableCameraMovement(const bool condition);
	void resetCamera();

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
	

	void update();
	
};

