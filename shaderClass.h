#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

std::string get_file_contents(const char* filename);

class Shader
{
	public:
		//shaderProgram's ID
		GLuint ID;

		//Constructor for shaderProgram with vertex and fragment shaders (Fragment controls color, Vertex controls position)
		Shader(const char* vertexFile, const char* fragmentFile);

		//Activates shaderProgram
		void Activate();
		//Deletes shaderProgram
		void Delete();
private:
	//Error check
	void compileErrors(unsigned int shader, const char* type);

};

#endif
