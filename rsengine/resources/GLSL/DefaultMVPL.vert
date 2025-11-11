#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform vec3 light_dir;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float _diffuse;
void main()
{
    vec3 Worldnormal;
    vec3 worldLightPos;
    vec3 mlight_Dir = vec3(0, 0, 0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Worldnormal = mat3(model) * aNormal;
    
    mlight_Dir = normalize(light_dir);
    Worldnormal = normalize(Worldnormal);
    float diff = max(dot(Worldnormal, -mlight_Dir), 0.0);

    _diffuse = diff;
}

