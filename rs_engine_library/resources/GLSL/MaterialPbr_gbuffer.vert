#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

//uniform vec3 light_dir;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

void main()
{
    vec4 world_pos = model * vec4(aPos, 1.0);
    vs_out.FragPos = world_pos.xyz;
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;

    gl_Position = projection * view * world_pos ;

}

