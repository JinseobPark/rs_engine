#version 460 core
/******************************************************************************/
/*!
\file   ClothRender.frag
\author Jinseob Park
\date   2024/11/30

Fragment shader for cloth rendering.
Basic Phong/PBR shading for cloth material.

*/
/******************************************************************************/

//********************************************************************************
// Inputs
//********************************************************************************

in VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec2 tex_coord;
} fs_in;

//********************************************************************************
// Uniforms
//********************************************************************************

// Material
uniform vec3 u_cloth_color;
uniform float u_roughness;
uniform float u_metallic;

// Lighting
uniform vec3 u_light_pos;
uniform vec3 u_light_color;
uniform vec3 u_view_pos;
uniform float u_ambient_strength;

// Optional texture
uniform bool u_use_texture;
uniform sampler2D u_cloth_texture;

//********************************************************************************
// Outputs
//********************************************************************************

out vec4 FragColor;

// For Deferred rendering G-Buffer output
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

//********************************************************************************
// Constants
//********************************************************************************

const float PI = 3.14159265359;

//********************************************************************************
// PBR Functions
//********************************************************************************

// Normal Distribution Function (GGX/Trowbridge-Reitz)
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

// Geometry Function (Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

// Fresnel (Schlick approximation)
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    // Get base color
    vec3 albedo = u_cloth_color;
    if (u_use_texture)
    {
        albedo *= texture(u_cloth_texture, fs_in.tex_coord).rgb;
    }
    
    // Normal (double-sided)
    vec3 N = normalize(fs_in.normal);
    if (!gl_FrontFacing)
        N = -N;
    
    vec3 V = normalize(u_view_pos - fs_in.frag_pos);
    vec3 L = normalize(u_light_pos - fs_in.frag_pos);
    vec3 H = normalize(V + L);
    
    // Calculate reflectance at normal incidence
    vec3 F0 = vec3(0.04);  // Dielectric
    F0 = mix(F0, albedo, u_metallic);
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, u_roughness);
    float G = GeometrySmith(N, V, L, u_roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    // Energy conservation
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - u_metallic;
    
    // Final radiance
    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = (kD * albedo / PI + specular) * u_light_color * NdotL;
    
    // Ambient
    vec3 ambient = u_ambient_strength * albedo;
    
    vec3 color = ambient + Lo;
    
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    
    // Gamma correction
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
    
    // G-Buffer output for deferred rendering
    gPosition = vec4(fs_in.frag_pos, 1.0);
    gNormal = vec4(N, 0.0);
    gAlbedoSpec = vec4(albedo, u_roughness);
}
