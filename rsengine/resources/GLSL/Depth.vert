#version 460
layout (location = 0) in vec3 aPos; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Position;
out vec2 TexCoords;
out vec3 Normal;


void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    
}
