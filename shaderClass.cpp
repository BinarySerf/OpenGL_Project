#include"shaderClass.h"

//Function for reading a text file into a string (I honestly don't fully understand this)
std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

//Constructor for shaderProgram with vertex and fragment shaders (Fragment controls color, Vertex controls position)
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	//Read vertexFile and fragmentFile
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	//Convert strings into char arrays
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	//Create the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Connect the vertexFile array to the vertex shader
	glShaderSource(vertexShader, 1, &vertexSource, NULL);

	//Compile vertex shader
	glCompileShader(vertexShader);

	//Error check
	compileErrors(vertexShader, "VERTEX");

	//Do all the same things for fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

	glCompileShader(fragmentShader);

	compileErrors(fragmentShader, "FRAGMENT");

	//Create the shaderProgram and store its ID
	ID = glCreateProgram();

	//Attatch the shaders
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);

	//Link the shaders together
	glLinkProgram(ID);

	//Error check
	compileErrors(ID, "PROGRAM");

	//Delete shader objects since they're now stored in shaderProgram
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

//Activates shaderProgram
void Shader::Activate()
{
	glUseProgram(ID);
}

//Deletes shaderProgram
void Shader::Delete()
{
	glDeleteProgram(ID);
}

//Error check for shader compilation
void Shader::compileErrors(unsigned int shader, const char* type)
{
	//Boolean for whether or not the program compiled
	GLint hasCompiled;

	//Array for storing error message
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		//Checks that each shader compiled correctly
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			//Type will tell which shader couldn't compile (either fragment or vertex)
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << std::endl;
		}
	}
	else
	{
		//Checks that shaders were able to be linked into the program
		glGetProgramiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			//Type will tell which shader couldn't compile (either fragment or vertex)
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << std::endl;
		}
	}
}