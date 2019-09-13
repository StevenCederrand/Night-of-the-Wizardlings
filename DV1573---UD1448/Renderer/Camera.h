#pragma once
#include <Pch/Pch.h>



class Camera
{
private:
	void calcVectors();


	bool firstMouse = true;
	double xpos, ypos;
	float lastX, lastY;

	void mouse_callback(GLFWwindow* window);

public:
	Camera();
	~Camera();

	void fpsControls(GLFWwindow* window, float deltaTime);
	void setWindowSize(float width, float height);
	void mouseControls(float xOffset, float yOffset, bool pitchLimit);
	void setProjMat(float widht, float height, float nearPlane, float farPlane);
	
	glm::mat4 getViewMat() const;
	glm::mat4 getProjMat() const;

	void update(GLFWwindow* window);


public:
	glm::vec3 camPos;
	glm::vec3 camFace;
	glm::vec3 worldUp;
	glm::vec3 camUp;
	glm::vec3 camRight;

	glm::mat4 projMat;

	float camYaw;
	float camPitch;
	float width;
	float height;
	float nearPlane;
	float farPlane;
	float camSpeed;
	float sensitivity;
	
	
};

