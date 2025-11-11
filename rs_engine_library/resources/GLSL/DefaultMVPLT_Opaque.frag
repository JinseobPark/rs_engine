#version 460
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform vec3 color;
uniform sampler2D tex;
uniform sampler2D shadow_map;

uniform vec3 light_pos;
uniform vec3 view_pos;

uniform float opacity = 1.0;
uniform bool use_transparent = false;

uniform mat4 tex_matrix;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //float closestDepth = texture(shadow_map, projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light_pos - fs_in.FragPos);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);
    //float bias = 0.001f;
    // check whether current frag pos is in shadow
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    for(int x_ = -1; x_ <= 1; ++x_)
    {
        for(int y_ = -1; y_ <= 1; ++y_)
        {
            float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x_, y_) * texelSize).r; 
//            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        // hard shadow
            shadow += currentDepth - bias > pcfDepth  ? pcfDepth : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{
    // adjust texture coordination
    vec4 texcoord = tex_matrix * vec4(fs_in.TexCoords, 0.0, 1.0);

    vec2 texcoord_flipy = vec2(texcoord.x, 1.0 - texcoord.y);
//    vec2 texcoord_flipy = vec2(fs_in.TexCoords.x, 1.0 - fs_in.TexCoords.y);
    


    vec4 tex_color = texture(tex, texcoord_flipy);
        if(tex_color.a < 0.1) 
        discard;

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.5);
    // ambient
    vec3 ambient = 0.7 * lightColor;
    // diffuse
    vec3 lightDir = normalize(light_pos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(view_pos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * tex_color.xyz * color;    
    //vec3 lighting = (ambient + (1.0 - shadow) * diffuse) * tex_color.xyz * color;    
    

	FragColor = vec4(lighting, 1.0);
    if(use_transparent)
    {
		FragColor = vec4(tex_color.xyz* color, opacity);
    }
}