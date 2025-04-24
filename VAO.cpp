#include"VAO.h"

//A VAO stores vertex pointers to the VBO
//Constructor for generating VAO (Vertex Array Object) ID
VAO::VAO()
{
	glGenVertexArrays(1, &ID);
}

//Link VBO attribute (color, position, etc.) to VAO
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();
	//Layout = vertex attribute position, numComponents = the number of values per vertex, type = type of values, stride = amount of data between vertices, offset = where vertices begin in array
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

//Binds VAO
void VAO::Bind()
{
	glBindVertexArray(ID);
}

//Unbinds VAO
void VAO::Unbind()
{
	glBindVertexArray(0);
}

//Deletes VAO
void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}