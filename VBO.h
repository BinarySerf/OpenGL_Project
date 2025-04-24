#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>


class VBO
{
public:
	//Generates and links VBO (Vertex Buffer Object) to vertices
	GLuint ID;
	VBO(GLfloat* vertices, GLsizeiptr size);

	//Binds VBO
	void Bind();
	//Unbinds VBO
	void Unbind();
	//Deletes VBO
	void Delete();

};

#endif