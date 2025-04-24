#include"VBO.h"

//A VBO is an array that can store a large number of vertices
//Generates and links VBO (Vertex Buffer Object) to vertices
VBO::VBO(GLfloat* vertices, GLsizeiptr size)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

//Binds VBO
void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}
//Unbinds VBO
void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
//Deletes VBO
void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}