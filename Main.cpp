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
"in vec3 color;\n"			//Read in color from vertex shader
"in vec2 texCoord;\n"		//Read in texture coords from vertex shader
"out vec4 FragColor;\n"		//Color output
"uniform sampler2D tex0;\n"	//Gets texture unit
"void main() {\n"
//Set fragment color to the texture with the texture unit and coords
"   FragColor = texture(tex0, texCoord);\n"
"}\n";

const char* ditheringShaderSource = R"glsl(
#version 460 core

in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D tex0;

// 2x2 Bayer Matrix
float thresholdMatrix(vec2 pos)
{
    int x = int(mod(pos.x, 2.0));
    int y = int(mod(pos.y, 2.0));
    
    float thresholds[2][2] = float[2][2](
        float[2](0.25, 0.75),
        float[2](1.00, 0.50)
    );
    
    return thresholds[y][x];
}

void main()
{
    vec4 color = texture(tex0, texCoord);
    vec2 fragCoord = gl_FragCoord.xy;
    float brightness = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    float threshold = thresholdMatrix(fragCoord);
    float finalColor = brightness > threshold ? 1.0 : 0.0;
    FragColor = vec4(vec3(finalColor), 1.0);
}
)glsl";

const char* rippleShaderSource = R"glsl(
#version 460 core

in vec2 texCoord;
in vec3 color;
out vec4 FragColor;

uniform sampler2D tex0;   
uniform vec2 mousePos;        
uniform float time;

void main()
{
    vec2 uv = texCoord;
    float dist = distance(uv, mousePos);
    float ripple = sin(30.0 * dist - time * 5.0) * 2;
    ripple *= exp(-10.0 * dist);
    vec2 offset = normalize(uv - mousePos) * ripple * 0.02;
    vec2 finalUV = uv + offset;
    FragColor = texture(tex0, finalUV);
}
)glsl";

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

//Shader Compilation function
GLuint compileShader(GLenum type, const char* source) {

	//Create shader
	GLuint shader = glCreateShader(type);
	
	//Attatch fragment or vertex shader source code
	glShaderSource(shader, 1, &source, NULL);
	//Compile shader
	glCompileShader(shader);

	//Check for compilation success
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

//Create the shader program
GLuint createShaderProgram(const char* vertSource, const char* fragSource) {
	//Compile vertex shader
	GLuint vertex = compileShader(GL_VERTEX_SHADER, vertSource);
	//Compile fragment shader
	GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragSource);

	//Create shader program
	GLuint shaderProgram = glCreateProgram();

	//Attach shaders
	glAttachShader(shaderProgram, vertex);
	glAttachShader(shaderProgram, fragment);

	//Link shaders
	glLinkProgram(shaderProgram);

	//Delete shaders once they are linked in the program
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return shaderProgram;
}

//Create a texture from a given file
GLuint createTexture(const char* filename) {
	//OpenGL and stb load images opposite to each other, so we have to flip image vertically
	stbi_set_flip_vertically_on_load(true);

	//Store width, height, and number of color channels of image
	int width, height, channels;
	//Store the actual image
	unsigned char* bytes = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
	//Create unsigned int for texture
	GLuint tex;

	//Generate texture object
	glGenTextures(1, &tex);
	//Bind texture (Binding is essentially just setting the object as a global variable so that it can be modified)
	glBindTexture(GL_TEXTURE_2D, tex);

	//Attatch texture image to bound object with texture target, mipmap level, texture format, width, height, legacy 0, source image format and datatype, and the actual image data)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	//Mipmaps are smaller sized renderings of an image so that it can be viewed from far away
	glGenerateMipmap(GL_TEXTURE_2D);

	//Set texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Free image memory and return
	stbi_image_free(bytes);
	return tex;
}

int main() 
{
	//GLFW initialization (GLFW is a library for creating windows and contexts)
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

	//GLAD initialization (GLAD is a library that provides function pointers for openGL)
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	//Set viewport to start from x,y = 0 to x = width, y = height
	glViewport(0, 0, width, height);

	//Create shader programs with vertex and fragment shaders
	GLuint baseShader = createShaderProgram(vertexShaderSource, fragmentShaderSource);
	GLuint ditheringShader = createShaderProgram(vertexShaderSource, ditheringShaderSource);
	GLuint rippleShader = createShaderProgram(vertexShaderSource, rippleShaderSource);

	int activeShader = 0;  // 0 = basic, 1 = dithering, 2 = ripple

	//Create unsigned int for VAO, VBO, and EBO
	// EBO (Elements buffer object) stores a large amount of data on indices
	// VBO (Vertex Buffer Object) stores a large amount of data on vertices
	//VAO (Vertex array object) acts as a middleman between the shader program and buffer objects by storing pointers to the vertex/index data)
	GLuint VAO, VBO, EBO;

	//Generate VAO
	glGenVertexArrays(1, &VAO);
	//Generate VBO
	glGenBuffers(1, &VBO);
	//Generate EBO
	glGenBuffers(1, &EBO);

	//Bind VAO
	glBindVertexArray(VAO);
	//Bind VBO & initialize it with vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Bind EBO & initialize it with index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Links VBO attributes to VAO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //Texture coords
	glEnableVertexAttribArray(2);


	//Texture initialization
	GLuint texture = createTexture("kirby.jpg");
	glUseProgram(activeShader);
	glUniform1i(glGetUniformLocation(activeShader, "tex0"), 0);


	//Rotation Variables (We don't need to make the pyramid/square spin in the final version)
	float rotation = 0.0f;
	double prevTime = glfwGetTime();

	//Enable depth buffer
	glEnable(GL_DEPTH_TEST);
	





	//While loop that keeps the window running until it's closed
	while (!glfwWindowShouldClose(window))
	{

		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
			activeShader = 0;
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
			activeShader = 1;
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
			activeShader = 2;


		//Create background color
		glClearColor(0.07f, 0.2f, 0.07f, 1.0f);
		//Clear color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		GLuint shader = baseShader;
		if (activeShader == 1)
			shader = ditheringShader;
		else if (activeShader == 2)
			shader = rippleShader;


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
	glDeleteProgram(baseShader);
	glDeleteProgram(ditheringShader);
	glDeleteProgram(rippleShader);

	//Delete window and terminate glfw before ending program
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}