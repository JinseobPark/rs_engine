#version 460
out vec4 FragColor;

uniform vec3 color = vec3(1.0);

void main()
{
    FragColor = vec4(vec3(color), 1.0f);
}