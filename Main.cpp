#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

//Set width and height for the window
const unsigned int width = 800;
const unsigned int height = 800;



// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"		//Position coords
"layout (location = 1) in vec3 aColor;\n"	//Color coords
"layout (location = 2) in vec2 aTex;\n"		//Texture coords
"out vec3 color;\n"							//Output color to fragment shader
"out vec2 texCoord;\n"						//Output texture to fragment shader
"uniform float scale;\n"					//Sets vertex scale
"uniform mat4 model;\n"						//Model matrix turns local coords into world coords
"uniform mat4 view;\n"						//View matrix turns world coords into view coords
"uniform mat4 proj;\n"						//Projection matrix turns view coords into screen coords

"void main() {\n"
//Outputs coordinates of the vertices
"   gl_Position = proj * view * model * vec4(aPos * scale, 1.0);\n"
//Send colors in vertex data to fragment shader
"   color = aColor;\n"
//Send texture coords in vertex data to fragment shader
"   texCoord = aTex;\n"
"}\n";



// Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 color;\n"
"in vec2 texCoord;\n"
"out vec4 FragColor;\n"
"uniform sampler2D tex0;\n"
"void main() {\n"
"   FragColor = texture(tex0, texCoord);\n"
"}\n";

//Initialize shape coords, color coords, and texture coords of the object
GLfloat vertices[] =
{ //     COORDINATES     /        COLORS         /   TexCoord  //
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
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

GLuint compileShader(GLenum type, const char* source) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	//Error Check
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("Shader Compilation Error:\n%s\n", infoLog);
	}
	return shader;
}

GLuint createShaderProgram(const char* vertSource, const char* fragSource) {
	GLuint vertex = compileShader(GL_VERTEX_SHADER, vertSource);
	GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragSource);
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	return program;
}

GLuint createTexture(const char* filename) {
	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	unsigned char* data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);
	return tex;
}

int main() 
{
	//GLFW initialization
	glfwInit();

	//I'm using openGL version 3.3, so we set GLFW context versions to 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//Set GLFW to use CORE profile (sets functions you can use), in this case we want modern fuctions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//Create window
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL_Test", NULL, NULL);

	//Error checking
	if (!window)
	{
		printf("GLFW Window failed to create");
		glfwTerminate();
		return -1;
	}
	//Use the window
	glfwMakeContextCurrent(window);

	//GLAD initialization
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	//Set viewport to start from x,y = 0 to x = width, y = height
	glViewport(0, 0, width, height);

	//Create shaderProgram with vertex and fragment shaders
	GLuint shader = createShaderProgram(vertexShaderSource, fragmentShaderSource);

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Links VBO attributes to VAO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	//Texture initialization
	GLuint texture = createTexture("kirby.jpg");
	glUseProgram(shader);
	glUniform1i(glGetUniformLocation(shader, "tex0"), 0);


	//Rotation Variables (We don't need to make the pyramid/square spin in the final version)
	float rotation = 0.0f;
	double prevTime = glfwGetTime();

	//Enable depth buffer
	glEnable(GL_DEPTH_TEST);
	
	//While loop that keeps the window running until it's closed
	while (!glfwWindowShouldClose(window))
	{
		//Create background color
		glClearColor(0.07f, 0.2f, 0.07f, 1.0f);
		//Clear color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Use shader program
		glUseProgram(shader);

		//Timer function
		double currTime = glfwGetTime();
		if (currTime - prevTime >= 1.0 / 60.0)
		{
			rotation += 0.5f;
			prevTime = currTime;
		}

		//Initialize matrices
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		//Assign matrix transformations
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
		proj = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 100.0f);

		//Outputs matrices to vertex shader
		int modelLoc = glGetUniformLocation(shader, "model");
		int viewLoc = glGetUniformLocation(shader, "view");
		int projLoc = glGetUniformLocation(shader, "proj");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		//Assign value to the uniform scale (meaning we're scaling by 100%, so nothing changes)
		glUniform1f(glGetUniformLocation(shader, "scale"), 1.0f);

		//Bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);

		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);
		//Swap front and back buffers
		glfwSwapBuffers(window);
		//Check for events and update window
		glfwPollEvents();
	}

	//Delete objects
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shader);

	//Delete window and terminate glfw before ending program
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}