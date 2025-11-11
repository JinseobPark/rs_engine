#version 460
out vec4 FragColor;

in float _diffuse;
uniform vec3 color;

void main()
{
    float ambient = 0.5f;
    
    FragColor = clamp(ambient + _diffuse*0.25f, 0.0, 1.0) * vec4(vec3(color), 1.0f);
}