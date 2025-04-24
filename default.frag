#version 330 core

//Outputs colors
out vec4 FragColor;

//Inputs color from vertex shader
in vec3 color;

//Inputs texture coords from vertex shader
in vec2 texCoord;

//Get texture from main
uniform sampler2D tex0;

void main()
{
	FragColor = texture(tex0, texCoord);
}