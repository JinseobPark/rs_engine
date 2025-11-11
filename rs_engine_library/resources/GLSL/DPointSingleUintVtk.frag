#version 460
layout (location = 3) out vec4 gPosition; // position + metallic
layout (location = 4) out vec4 gNormal;   // normal + roughness
layout (location = 5) out vec4 gAlbedoSpec; // albedo + ao
layout (location = 6) out vec4 gCustomDepth; // r : opaque, g : fluid. linear depth
layout (location = 7) out uvec4 gCustomIndex; // mask, nothing 3

in VS_OUT {
    float val_color;
    vec3 world_pos;
} fs_in;

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform float legend_min = 0;
uniform float legend_max = 83000;

uniform bool use_legend = false;
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
// ----------------------------------------------------------------------------
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

    //tex1 : legend texture
    float val = fs_in.val_color;
    // calculate val to 0~1 (min, max)
    float val_per = clamp(1.0 - ( (val - legend_min) / (legend_max - legend_min) ), 0.005, 0.995);
    // val is 0 ~ max_val. convert to 0~1 about texture x-axis coordinate
    vec2 legend_color = vec2(val_per, val_per);

//    // if the value is out of the legend range, then the color should be black.
//    if(val < legend_min || val > legend_max)
//    {
//        FragColor = vec4(0.2, 0.2, 0.2, 0.3);
//        return;
//    }

    vec4 tex1_color = texture(tex1, legend_color);
    
    gPosition = vec4(fs_in.world_pos, water_metallic);
    gAlbedoSpec = vec4(tex1_color.rgb * tex0_color.rgb, ambient);
    
    gCustomIndex = uvec4(CUSTOM_MASK_FLUID); // nothing
    gCustomDepth.g = LinearDepth(gl_FragCoord.z); // linear depth near to far
    gCustomDepth.a = 1.0;

}  