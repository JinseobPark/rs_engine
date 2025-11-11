#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D position_map; // position + metallic
uniform sampler2D normal_map;   // normal + roughness
uniform sampler2D albedo_map;   // albedo + ambient
uniform sampler2D fluid_position_map; // position + metallic
uniform sampler2D fluid_normal_map;   // normal + roughness
uniform sampler2D fluid_albedo_map;   // albedo + ambient
uniform sampler2D custom_depth_map;   // r : opaque depth, g : fluid depth
uniform usampler2D custom_index_map;     // mask + nothing
uniform sampler2D depth_map;    // depth map
uniform sampler2D shadow_map;
uniform sampler2D ssao_map;
uniform sampler2D ssr_map;
uniform samplerCube skybox;

uniform vec3 background_color;
uniform vec3 view_pos;
uniform vec3 light_pos;
uniform mat4 light_space_matrix;

uniform bool b_skybox_reflection;
uniform bool b_ssr_reflection;

uniform bool use_fluid_map = false;


const float PI = 3.14159265359;
const uint CUSTOM_MASK_FLUID = 200;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 normal, vec3 halfway_dir, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(normal, halfway_dir), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 normal, vec3 view_dir, vec3 light_dir, float roughness)
{
    float NdotV = max(dot(normal, view_dir), 0.0);
    float NdotL = max(dot(normal, light_dir), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
float ShadowCalculation(vec4 FragPosLightSpace)
{
    // perform perspective divide
    vec3 proj_coords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    // transform to [0,1] range
    proj_coords = proj_coords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //float closestDepth = texture(shadow_map, proj_coords.xy).r; 

    // get depth of current fragment from light's perspective
    float current_depth = proj_coords.z;
    // calculate bias (based on depth map resolution and slope)

//    vec3 normal = normalize(fs_in.Normal);
    vec3 tangent_normal = texture(normal_map, TexCoords).rgb;
    vec3 normal = normalize(tangent_normal * 2.0 - 1.0);

    vec3 FragPos = texture(position_map, TexCoords).rgb;

    vec3 light_dir = normalize(light_pos - FragPos);
    float bias = max(0.005 * (1.0 - dot(normal, light_dir)), 0.001);
    //float bias = 0.001f;
    // check whether current frag pos is in shadow
    //float shadow = current_depth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    for(int x_ = -1; x_ <= 1; ++x_)
    {
        for(int y_ = -1; y_ <= 1; ++y_)
        {
            float pcfDepth = texture(shadow_map, proj_coords.xy + vec2(x_, y_) * texelSize).r; 
//            shadow += current_depth - bias > pcfDepth  ? 1.0 : 0.0;        // hard shadow
            shadow += current_depth - bias > pcfDepth  ? pcfDepth : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(proj_coords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}
// ----------------------------------------------------------------------------

void main() {
    
    vec3 FragPos = texture(position_map, TexCoords).rgb;
    vec3 Normal = texture(normal_map, TexCoords).rgb;
    vec3 Albedo = texture(albedo_map, TexCoords).rgb;
    float metallic = texture(position_map, TexCoords).a;
    float roughness = texture(normal_map, TexCoords).a;
    float ambient = texture(albedo_map, TexCoords).a;
    float depth = texture(depth_map, TexCoords).r;
    float ssao = texture(ssao_map, TexCoords).r;
    vec4 FragPosLightSpace = light_space_matrix * vec4(FragPos, 1.0);
    uint custom_index = texture(custom_index_map, TexCoords).r; // mask + nothing
    vec4 custom_depth = texture(custom_depth_map, TexCoords); // r : opaque depth, g : fluid depth

    if(use_fluid_map && custom_index.r == CUSTOM_MASK_FLUID)
    {
        FragPos = texture(fluid_position_map, TexCoords).rgb;
        Normal = texture(fluid_normal_map, TexCoords).rgb;
        Albedo = texture(fluid_albedo_map, TexCoords).rgb;
        metallic = texture(fluid_position_map, TexCoords).a;
        roughness = texture(fluid_normal_map, TexCoords).a;
        ambient = texture(fluid_albedo_map, TexCoords).a;
    }

    

    if(depth >= 1.0)
    {
      FragColor = vec4(background_color, 1.0);
      return;
    }

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, metallic);


    vec3 lightDir = normalize(light_pos - FragPos);
    float diff = max(dot(lightDir, Normal), 0.0);
    vec3 viewDir = normalize(view_pos - FragPos);
    vec3 halfwayDir = normalize(viewDir + lightDir);

    float NDF = DistributionGGX(Normal, halfwayDir, roughness);
    float G = GeometrySmith(Normal, viewDir, lightDir, roughness);
    vec3 F = fresnelSchlick(clamp(dot(halfwayDir, viewDir), 0.0, 1.0), F0);

    vec3 numerator = NDF * G * F;
//    float denominator = 4 * max(dot(Normal, viewDir), 0.0) * max(dot(Normal, lightDir), 0.0);
    float denominator = 4 * max(dot(Normal, viewDir), 0.0) * max(dot(Normal, lightDir), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    // kS = fresnel
    vec3 kS = F;
    // kD = 1 - kS
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;


    // shadow
    float shadow = ShadowCalculation(FragPosLightSpace);

    // scale light by NdotL
    float NdotL = max(dot(Normal, lightDir), 0.0);

//    vec3 lighting = (kD * Albedo / PI + specular) * diff * NdotL;
    vec3 lighting = (kD * Albedo / PI + specular) * NdotL;
    
    vec3 specularIBL = vec3(0.0);

    vec3 base = ambient * Albedo * ssao;

    bool reflection_passed = false;
    if(b_ssr_reflection)
    {
        // ssr reflection
        float ssr_alpha = texture(ssr_map, TexCoords).a;
        vec3 prefilterColor = texture(ssr_map, TexCoords).rgb;
        specularIBL += prefilterColor * F * ssr_alpha;

        // If specularIBL is not zero, then reflection is passed
        if(specularIBL != vec3(0.0))
            reflection_passed = true;
    }

    // skybox reflection
    if(b_skybox_reflection && !reflection_passed)
    {
        vec3 I = normalize(FragPos - view_pos);
        vec3 R = reflect(I, normalize(Normal));
        vec3 prefilterColor = textureLod(skybox, R, roughness * 5.0).rgb;
        specularIBL += prefilterColor * F;
        reflection_passed = true;
    }
    vec3 final_color = base + min((1.0 - shadow) * lighting + specularIBL, 1.0 - base);
//    if(reflection_passed)
//        final_color = mix(final_color, specularIBL, 0.75);

    // HDR tonemapping
    final_color = final_color / (final_color + vec3(1.0));
    // gamma correction
    float gamma = 2.2;
    final_color = pow(final_color, vec3(1.0/gamma));

    FragColor = vec4(final_color, 1.0);
}