#include"Texture.h"

//Constructs texture function
Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	//Sets texture type to texture object
	type = texType;

	//Stores image variables: width, height, # of color channels
	int widthImg, heightImg, numColCh;

	//Since openGL reads images opposite to the way stb reads them, you need to flip image vertically
	stbi_set_flip_vertically_on_load(true);

	//Stores image file into bytes
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, STBI_rgb_alpha);

	//Generates texture object
	glGenTextures(1, &ID);
	//Assigns texture to texture unit (texture unit is a slot for textures to be accessed by shaders)
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	//Texture algorithm (GL_LINEAR makes it less pixalated but more blurry)
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Repeats the texture on both axis
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Sets image to the texture object
	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);

	//Generates MipMaps which creates smaller resolution duplicates of texture and is used when an image appears further away from camera
	glGenerateMipmap(texType);

	//Frees up bytes used by the image
	stbi_image_free(bytes);

	//Unbinds texture object
	glBindTexture(texType, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	//Get uniform location
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);

	//Activates shader to modify uniform
	shader.Activate();

	//Set uniform's value to unit
	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
	glBindTexture(type, ID);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}