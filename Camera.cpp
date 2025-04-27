#include "Camera.h"
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>


//declare variables
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float lastX = 400.0f, lastY = 400.0f;
float yaw = -90.0f;
float pitch = 0.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float fov = 35.0f;

bool mouseLocked = true;
bool keyboard = true;

void Camera::processInput(GLFWwindow* window, glm::vec3& cameraPos, glm::vec3& cameraFront, glm::vec3& cameraUp, float deltaTime)
{
	const float cameraSpeed = 2.5f* deltaTime; // adjust accordingly

	//toggle between freeform cam and locked cam
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
	{
		keyboard = true;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		keyboard = false;
	}
	
	//WASD inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (keyboard) {
			cameraPos += cameraSpeed * cameraFront;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (keyboard) {
			cameraPos -= cameraSpeed * cameraFront;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		if (keyboard) {
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		if (keyboard) {
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
	}

	//remove cursor from window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		if (mouseLocked)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			mouseLocked = false;
		}
	}
	//use cursor in window
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		if (!mouseLocked)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			firstMouse = true; // to avoid sudden jump
			mouseLocked = true;
		}
	}
	
}

void Camera::mouse_callback(GLFWwindow * window, double xpos, double ypos)
{


	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;

	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	if (keyboard) {
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);
	}
	
}

void Camera::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}