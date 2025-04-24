#version 330 core

//Coordinates
layout (location = 0) in vec3 aPos;
//Colors
layout (location = 1) in vec3 aColor;
//Texture coordinates
layout (location = 2) in vec2 aTex;

//Outputs color for frag shader
out vec3 color;
//Outputs texture coords to frag shader
out vec2 texCoord;

//Sets scale of vertices
uniform float scale;



//model matrix turns local coords into world coords
uniform mat4 model;

//view matrix turns world coords into view coords
uniform mat4 view;

//projection matrix turns view coords into screen coords
uniform mat4 proj;

void main()
{
	//Outputs coordinates of the vertices
	gl_Position = proj * view * model * vec4(aPos, 1.0);

	//Sets color as the colors in vertex data
	color = aColor;

	//Sets texCoord as the texture coords from vertex data
	texCoord = aTex;
}