#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "stb_image.h"


const char *vertexShaderSource;
const char *fragmentShaderSource;


// Log a value of a variable in a file
void logVariable(const char* name, char* value) {
    FILE* logFile = fopen("log.txt", "a");
    if (logFile) {
        fprintf(logFile, "(%s): %s\n", name, value);
        fclose(logFile);
    }
}
// Loads a glsl shader from a file into a string
char* get_shader_content(const char* fileName)
{
    FILE *fp;
    long size = 0;
    char* shaderContent;
    
    /* Read File to get size */
    fp = fopen(fileName, "rb");
    if(fp == NULL) {
        return "";
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp)+1;
    fclose(fp);

    /* Read File for Content */
    fp = fopen(fileName, "r");
    shaderContent = memset(malloc(size), '\0', size);
    fread(shaderContent, 1, size-1, fp);
    fclose(fp);

    return shaderContent;
}

// process the input of the winodw
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}


// used to resize the viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}  

int main(void) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Creates a window and initializes it to the current context
  GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Loads GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    printf("Failed to initialize GLAD");
    return -1;
  }    

  // set size of the rendering window
  glViewport(0, 0, 800, 600);

  // resize window viewport on window resize
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // --- PRE RENDERING SHENANIGANS ---


  float vertices[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
  };
  unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
 };  

  // --- GLSL OBJECTS ---
  int  success;
  char infoLog[512];

  // Creating a Vertex Array Object :: VAO
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);  
  glBindVertexArray(VAO);
  
  // Vertext buffer object
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);  
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Sending vertices to the Virtual Buffer Object 



  // Vertex Shader + Error checking at compile
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  vertexShaderSource = get_shader_content("shaders/vertexShader.glsl");
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s", infoLog);
    return -1;
  }
  // Fragment Shader
  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  fragmentShaderSource = get_shader_content("shaders/dither_shader.glsl"); // Import shader
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);


  // Creating EBO element buffer object,    IDK WHERE TF THIS GOES
  unsigned int EBO;
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 


  // Linking Vertext Attributes 
  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

  // textures
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);  


  // unbind VBO and VAO now that they are bound in the EBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);


 



  // Shader Program  
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);


 glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog); 
    printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s", infoLog);
    return -1;
  }


  // Delete shader objects once we have the shaders loaded
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);  


  // Implementing Textures
  float texCoords[] = {
    0.0f, 0.0f,  // lower-left corner  
    1.0f, 0.0f,  // lower-right corner
    0.5f, 1.0f   // top-center corner
  };


  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); // S and T denote coordniate spaces like X Y and Z

  float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); // This is used if you are using the GL_BOREDER_CLAMP
  
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // MIPMAP filtering options
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  unsigned int texture;
  glGenTextures(1, &texture); // bind texture
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  int width, height, nrChannels;
  unsigned char *data = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else {
    printf("Failed to load texture"); // i honestly doubt this will print cuz prinf is retarded
    logVariable("texture", data);
  }
  stbi_image_free(data);   // free image, not tex





  // -----  RENDER LOOP -----
  int temp;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &temp);
  while(!glfwWindowShouldClose(window)) {
    // input here
    processInput(window);

    // rendering here
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // activate the shader to connect the uniform
    //glUseProgram(shaderProgram);


    // ---------------------------
    // Assume you already have 'shaderProgram' as your linked shader program

    double mouseX, mouseY;
    int windowWidth, windowHeight;

    // Get mouse position
    glfwGetCursorPos(window, &mouseX, &mouseY);
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // Normalize to (0, 1) range
    float normalizedMouseX = (float)mouseX / windowWidth;
    float normalizedMouseY = 1.0f - (float)mouseY / windowHeight; // Flip Y!

    // Get time
    float timeValue = glfwGetTime();

    // Set the uniforms
    glUseProgram(shaderProgram);
    int mousePosLocation = glGetUniformLocation(shaderProgram, "mousePos");
    glUniform2f(mousePosLocation, normalizedMouseX, normalizedMouseY);

    int timeLocation = glGetUniformLocation(shaderProgram, "time");
    glUniform1f(timeLocation, timeValue);
    // --------------------------------------



    // render the triangle
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // check and call events / swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();    
  }

  glfwTerminate();
  return 0;
}
//  FILE *logFile = fopen("app.log", "a");
//  if (logFile == NULL) {
//    perror("Error opening log file");
//    return 1; 
//  }
//  fprintf(logFile, fragShaderSource);
//  fclose(logFile);

