#pragma once
#include <Pch/Pch.h>



class Camera
{
private:
	void calcVectors();
	
	bool firstMouse = true;
	double xpos, ypos;
	float lastX, lastY;

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

	void mouse_callback(GLFWwindow* window);

public:
	Camera();
	~Camera();

	void fpsControls(float deltaTime);
	void setWindowSize(float width, float height);
	void mouseControls(float xOffset, float yOffset, bool pitchLimit);
	void setProjMat(float widht, float height, float nearPlane, float farPlane);
	void setCameraPos(glm::vec3 pos);

	const glm::mat4 getViewMat() const;
	const glm::mat4& getProjMat() const;
	double getXpos() const;
	double getYpos() const;

	glm::vec3 getCamFace(); //trashkod
	glm::vec3 getCamRight();
	const glm::vec3& getPosition() const;
	const glm::vec3& getDirection() const;

	GLFWwindow* getWindow();


	void update(GLFWwindow* window);
	
};

