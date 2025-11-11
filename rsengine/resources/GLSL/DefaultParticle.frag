#version 460

uniform vec3 color;

out vec4 FragColor;

void main()
{   
    vec2 temp = gl_PointCoord - vec2(0.5);
    float f = dot(temp, temp);
    if (f>0.25) discard;
    FragColor = vec4(vec3(color), 1.0f);
}  