#version 460 core
/******************************************************************************/
/*!
\file   ClothRender.vert
\author Jinseob Park
\date   2024/11/30

Vertex shader for cloth rendering.
Reads position and normal from SSBOs.

*/
/******************************************************************************/

//********************************************************************************
// SSBO Bindings (read-only for rendering)
//********************************************************************************

layout (std430, binding = 10) readonly buffer PositionBuffer {
    vec4 positions[];  // xyz: position, w: inverse mass
};

layout (std430, binding = 14) readonly buffer NormalBuffer {
    vec4 normals[];  // xyz: normal, w: unused
};

//********************************************************************************
// Uniforms
//********************************************************************************

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform int cloth_width;

//********************************************************************************
// Outputs
//********************************************************************************

out VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec2 tex_coord;
} vs_out;

void main()
{
    // gl_VertexID is the index buffer value
    uint particle_id = gl_VertexID;
    
    vec3 pos = positions[particle_id].xyz;
    vec3 norm = normals[particle_id].xyz;
    
    // Calculate texture coordinates from grid position
    int x = int(particle_id) % cloth_width;
    int y = int(particle_id) / cloth_width;
    vs_out.tex_coord = vec2(float(x) / float(cloth_width - 1), 
                            float(y) / float(cloth_width - 1));
    
    // Transform
    vec4 world_pos = u_model * vec4(pos, 1.0);
    vs_out.frag_pos = world_pos.xyz;
    vs_out.normal = mat3(transpose(inverse(u_model))) * norm;
    
    gl_Position = u_projection * u_view * world_pos;
}
