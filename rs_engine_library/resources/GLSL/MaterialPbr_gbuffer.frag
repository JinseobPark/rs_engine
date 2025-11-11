#version 460
layout (location = 0) out vec4 gPosition; // position + metallic
layout (location = 1) out vec4 gNormal;   // normal + roughness
layout (location = 2) out vec4 gAlbedoSpec; // albedo + dummy(nothing)
layout (location = 3) out uvec4 gCustomIndex; // mask, nothing 3

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuse_map;
uniform sampler2D normal_map;

uniform vec3 color;

uniform float metallic;
uniform float roughness;
uniform float ambient = 0.3;

uniform mat4 tex_matrix;

const uint CUSTOM_MASK_OPAQUE = 100;

// ----------------------------------------------------------------------------
vec3 GetNormalFromMap()
{
    vec4 texcoord = tex_matrix * vec4(fs_in.TexCoords, 0.0, 1.0);
    vec2 texcoord_flipy = vec2(texcoord.x, 1.0 - texcoord.y);

    vec3 tangent_normal = texture(normal_map, texcoord_flipy).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.FragPos);
    vec3 Q2  = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangent_normal);
}
// ----------------------------------------------------------------------------
void main()
{
    // adjust texture coordination
    vec4 texcoord = tex_matrix * vec4(fs_in.TexCoords, 0.0, 1.0);

    vec2 texcoord_flipy = vec2(texcoord.x, 1.0 - texcoord.y);
    
    vec4 tex_color = texture(diffuse_map, texcoord_flipy);
        if(tex_color.a < 0.1) 
        discard;

    gPosition.xyz = fs_in.FragPos;
    gPosition.w = metallic;

    gNormal.xyz = GetNormalFromMap();
    gNormal.w = roughness;

    gAlbedoSpec.xyz = tex_color.rgb * color;
    gAlbedoSpec.w = ambient;

    gCustomIndex = uvec4(CUSTOM_MASK_OPAQUE); // nothing
//    gCustomIndex.r = CUSTOM_MASK_OPAQUE; // Opaque mask
}