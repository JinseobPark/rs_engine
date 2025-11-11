#version 460
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuse_map;
uniform sampler2D normal_map;
uniform sampler2D shadow_map;

uniform vec3 color;
uniform vec3 light_pos;
uniform vec3 view_pos;

uniform float opacity = 1.0;
uniform float metallic;
uniform float roughness;
uniform bool use_transparent = false;

uniform mat4 tex_matrix;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
vec3 getNormalFromMap()
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
    vec3 normal = normalize(fs_in.Normal);
    vec3 light_dir = normalize(light_pos - fs_in.FragPos);
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

void main()
{
    // adjust texture coordination
    vec4 texcoord = tex_matrix * vec4(fs_in.TexCoords, 0.0, 1.0);

    vec2 texcoord_flipy = vec2(texcoord.x, 1.0 - texcoord.y);
//    vec2 texcoord_flipy = vec2(fs_in.TexCoords.x, 1.0 - fs_in.TexCoords.y);
    
    vec4 tex_color = texture(diffuse_map, texcoord_flipy);
        if(tex_color.a < 0.1) 
        discard;

    vec3 F0 = vec3(0.04); 
     F0 = mix(F0, color, metallic);
    vec3 normal = getNormalFromMap();
//    vec3 lightColor = vec3(0.5);
    // diffuse
    vec3 light_dir = normalize(light_pos - fs_in.FragPos);
    float diff = max(dot(light_dir, normal), 0.0);
//    vec3 diffuse = diff * lightColor;
    // specular
    vec3 view_dir = normalize(view_pos - fs_in.FragPos);
//    vec3 reflectDir = reflect(-light_dir, normal);
    vec3 halfway_dir = normalize(light_dir + view_dir);  

    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, halfway_dir, roughness);   
    float G   = GeometrySmith(normal, view_dir, light_dir, roughness);      
    vec3 F    = fresnelSchlick(clamp(dot(halfway_dir, view_dir), 0.0, 1.0), F0);
           
    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(normal, view_dir), 0.0) * max(dot(normal, light_dir), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
        
    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;	  

    // calculate shadow

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    
    // scale light by NdotL
    float NdotL = max(dot(normal, light_dir), 0.0);        

    // add to outgoing radiance Lo

    vec3 lighting = (kD * color / PI + specular) * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    //(ambient + (1.0 - shadow) ) * tex_color.xyz * color;    

    // vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * tex_color.xyz * color;    
    
    
    // ambient
    vec3 ambient = vec3(0.2f);

    vec3 final_color = (ambient + (1.0 - shadow) * lighting) * tex_color.xyz * color;    
//    vec3 final_color = ambient + lighting;
    
    // HDR tonemapping
    final_color = final_color / (final_color + vec3(1.0));
    // gamma correct
    final_color = pow(final_color, vec3(1.0/2.2)); 

	FragColor = vec4(final_color, 1.0);
    if(use_transparent)
    {
		FragColor = vec4(tex_color.xyz* color, opacity);
    }
}