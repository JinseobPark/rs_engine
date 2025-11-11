#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float alpha;

void main()
{   
    
    FragColor = texture(skybox, TexCoords);
    FragColor.a = clamp(alpha, 0.0, 0.5);
}