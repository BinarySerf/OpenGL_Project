#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include<glad/glad.h>
#include"VBO.h"


class VAO
{
public:
	//Sets ID for VAO (Vertex Array Object) and generates it
	GLuint ID;
	VAO();

	//Link VBO attribute (color, position, etc.) to VAO
	void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);

	//Binds VAO
	void Bind();
	//Unbinds VAO
	void Unbind();
	//Deletes VAO
	void Delete();

};

#endif