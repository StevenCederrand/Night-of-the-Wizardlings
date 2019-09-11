#pragma once
#include <Pch/Pch.h>

class Camera
{
private:
	void calcVectors();

public:
	Camera();
	~Camera();

	void fpsControls(GLFWwindow* window, float deltaTime);
	void setWindowSize(float width, float height);
	void mouseControls(float xOffset, float yOffset, bool pitchLimit);
	void setProjMat(float widht, float height, float nearPlane, float farPlane);
	
	glm::mat4 getViewMat();
	glm::mat4 getProjMat();


public:
	glm::vec3 camPos,
		camFace,
		worldUp,
		camUp,
		camRight;

	glm::mat4 projMat;

	float camYaw,
		camPitch,
		width,
		height,
		nearPlane,
		farPlane,
		camSpeed,
		sensitivity;


	
};

