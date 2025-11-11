#version 460
layout (location = 3) out vec4 gPosition; // position + metallic
layout (location = 4) out vec4 gNormal;   // normal + roughness
layout (location = 5) out vec4 gAlbedoSpec; // albedo + ao
layout (location = 6) out vec4 gCustomDepth; // r : opaque, g : fluid. linear depth
layout (location = 7) out uvec4 gCustomIndex; // mask, nothing 3

in VS_OUT {
    vec3 val_color;
    vec3 world_pos;
} fs_in;

uniform sampler2D tex0;
uniform vec3 value_min;
uniform vec3 value_max;

uniform mat4 view;
uniform float near;
uniform float far;
uniform float ambient = 0.8;

//const float water_metallic = 0.0;
const float water_metallic = 0.8; // for debugging
//const float water_roughness = 0.01;
const float water_roughness = 0.2; // for debugging 
const float water_IOR = 1.333;
const float water_specular = 0.25;
const uint CUSTOM_MASK_FLUID = 200;

float LinearDepth(float depth)
{
    // Calculate the linear depth from the view matrix and projection matrix
    float z = depth * 2.0 - 1.0; // Convert to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}


void main()
{   
    vec2 pointUV = gl_PointCoord;
    
    // Calculate normal vector from point coordinates
    vec2 normalPointUV = vec2(pointUV.x, 1.0 - pointUV.y);
    vec3 normal;
    normal.xy = normalPointUV * 2.0 - 1.0;

    // Check if point is within unit circle
    float r2 = dot(normal.xy, normal.xy);
    if (r2 > 1.0) discard; // Discard fragments outside the circle

    vec4 tex0_color = texture(tex0, pointUV);

    // Calculate z component using hemisphere formula
    normal.z = sqrt(1.0 - r2) * 2.0 - 1.0;

    // Transform from view space to world space
    vec4 worldNormal = inverse(view) * vec4(normal, 0.0);
    vec3 viewNormalDir = normalize(worldNormal.xyz);

    // Output normal and roughness value
    gNormal = vec4(viewNormalDir, water_roughness);

    vec3 value_color = fs_in.val_color;
    // value_color should changed 0~1 by value_min~value_max for xyz
    vec3 min_max_diff = value_max - value_min;

    // When any element of diff is 0, then the element should be 0.1.
    min_max_diff = max(min_max_diff, vec3(0.1, 0.1, 0.1));

    value_color = (value_color - value_min) / min_max_diff;

//    // if any the value is out of the value_min and value_max, then the color should be black.
//    if(value_color.r < 0.0 || value_color.r > 1.0 || value_color.g < 0.0 || value_color.g > 1.0 || value_color.b < 0.0 || value_color.b > 1.0)
//    {
//        value_color = vec4(0.2, 0.2, 0.2, 0.3);
//    }


    gPosition = vec4(fs_in.world_pos, water_metallic);
    gAlbedoSpec = vec4(value_color * tex0_color.rgb, ambient);
    
    gCustomIndex = uvec4(CUSTOM_MASK_FLUID); // nothing
    gCustomDepth.g = LinearDepth(gl_FragCoord.z); // linear depth near to far
    gCustomDepth.a = 1.0;

}  