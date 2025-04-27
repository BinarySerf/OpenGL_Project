#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"

//Set width and height for the window
const unsigned int width = 800;
const unsigned int height = 800;

//Initialize shape coords, color coords, and texture coords of the object
GLfloat vertices[] =
{ //     COORDINATES         /           COLORS             /    TexCoord    //
	-0.5f, 0.0f,  0.5f,      0.83f, 0.70f, 0.44f,    0.0f, 0.0f,
	-0.5f, 0.0f, -0.5f,      0.83f, 0.70f, 0.44f,    5.0f, 0.0f,
	 0.5f, 0.0f, -0.5f,      0.83f, 0.70f, 0.44f,    0.0f, 0.0f,
	 0.5f, 0.0f,  0.5f,      0.83f, 0.70f, 0.44f,    5.0f, 0.0f,
	 0.0f, 0.8f,  0.0f,      0.92f, 0.86f, 0.76f,    2.5f, 5.0f
};

// Indices for vertex order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};


int main()
{

	
	//GLFW initialization
	glfwInit();

	//I'm using openGL version 3.3, so we set GLFW context versions to 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//Set GLFW to use CORE profile (sets functions you can use), in this case we want modern fuctions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//Create Camera object
	Camera controls;

	//Create window
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL_Test", NULL, NULL);

	//Error checking
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//Use the window
	glfwMakeContextCurrent(window);

	

	glfwSetCursorPosCallback(window, controls.mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, controls.scroll_callback);


	//GLAD initialization
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Set viewport to start from x,y = 0 to x = width, y = height
	glViewport(0, 0, width, height);

	//Create shaderProgram with vertex and fragment shaders
	Shader shaderProgram("default.vert", "default.frag");

	//Create VAO (Vertex Array Object)
	VAO VAO1;
	//Binds VAO1 (Binding is essentially setting an object as the current one so it can be utilized/modified)
	VAO1.Bind();

	//Create VBO (Vertex Buffer Object) and link to vertices
	VBO VBO1(vertices, sizeof(vertices));

	//Create EBO (Element Buffer Object) and link to indices
	EBO EBO1(indices, sizeof(indices));

	//Links VBO attributes to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	//Unbind objects
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	//Sets the scale uniform ID
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");


	//Texture initialization
	Texture kirby("kirby.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	kirby.texUnit(shaderProgram, "tex0", 0);


	//Rotation Variables (We don't need to make the pyramid/square spin in the final version)
	float rotation = 0.0f;
	
	

	//Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	//Declare camera variables
	
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);



	//Initialize matrices
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);

	
	
	

	//While loop that keeps the window running until it's closed
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		//Call the processInput function
		controls.processInput(window, cameraPos, cameraFront, cameraUp, deltaTime);
		//Create background color
		glClearColor(0.07f, 0.2f, 0.07f, 1.0f);
		//Clear color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Use shader program
		shaderProgram.Activate();
		



		model = glm::mat4(1.0f);

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

		cameraUp = glm::cross(cameraDirection, cameraRight);

		view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);



		//Assign matrix transformations
		//model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
		proj = glm::perspective(glm::radians(fov), (float)(width / height), 0.1f, 100.0f);

		//Outputs matrices to vertex shader
		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		//Assign value to the uniform (meaning we're scaling by 50%)
		glUniform1f(uniID, 0.5f);
		//Bind texture
		kirby.Bind();
		//Bind VAO
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		//Swap front and back buffers
		glfwSwapBuffers(window);
		//Check for events and update window
		glfwPollEvents();
	}

	//Delete objects
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	kirby.Delete();
	shaderProgram.Delete();

	//Delete window and terminate glfw before ending program
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}