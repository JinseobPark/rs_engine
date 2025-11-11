#version 330 core
out vec4 color;

uniform vec4 solid_color;

void main()
{    
	color = vec4(solid_color);
}