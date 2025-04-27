#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include<glm/glm.hpp>
#include<GLFW/glfw3.h>

//Global variables
extern glm::vec3 cameraFront;
extern glm::vec3 cameraPos;
extern glm::vec3 cameraUp;
extern bool firstMouse, mouseLocked, keyboard;
extern float lastX, lastY, yaw, pitch, deltaTime, lastFrame, fov;

class Camera{
public:
	//input for WASD and other keyboard functions
	static void processInput(GLFWwindow* window, glm::vec3& cameraPos, glm::vec3& cameraFront, glm::vec3& cameraUp, float deltaTime);
	//mouse inputs
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	//zoom
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

};

#endif
