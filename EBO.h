#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include<glad/glad.h>


class EBO
{
public:
	//Set EBO's ID
	GLuint ID;

	//Constructor for generating EBO (Elements Buffer Object) and links it to indices
	EBO(GLuint* indices, GLsizeiptr size);

	void Bind();
	void Unbind();
	void Delete();

};

#endif