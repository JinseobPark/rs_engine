
#version 430 core
out vec4 final_reflection;
in vec2 TexCoords;
uniform sampler2D position_map;  // position + metallic
uniform sampler2D normal_map;    // normal(TBN)
uniform sampler2D albedo_map;    // albedo + ambient
uniform sampler2D depth_map;     // depth map
// Camera uniforms
uniform mat4 view;
uniform mat4 proj;

//uniform vec3 cam_pos;
// SSR parameters
uniform float max_distance = 50.0;     // Maximum ray distance
uniform int max_steps = 20;           // Maximum ray steps 
uniform int binary_search_steps = 5;   // Binary search refinement steps
uniform float reflection_strength = 1.0;
uniform float near = 0.1;             // Near plane distance
uniform float far = 1000.0;           // Far plane distance

uniform float g_max_thickness = 1.0;
uniform float g_depthbias = 0.1;

const float resolution = 0.3;
uniform vec2 screen_size;

// ----------------------------------------------------------------------------
vec2 WorldToScreen(vec3 world_pos)
{
    // Convert to clip space
    vec4 clip_space_pos = proj * view * vec4(world_pos, 1.0);
    // Perspective division to get NDC coordinates
    vec2 ndc = clip_space_pos.xy / clip_space_pos.w;
    // Convert from [-1,1] to [0,1] range
    return ndc * 0.5 + 0.5;
}
// ----------------------------------------------------------------------------
float GetLinearDepth(vec3 world_pos)
{
    vec4 clip_space_pos = proj * view * vec4(world_pos, 1.0);
    float ndc_depth = clip_space_pos.z / clip_space_pos.w;
    // Convert NDC depth to linear depth
    return (2.0 * near * far) / (far + near - ndc_depth * (far - near));
}
// ----------------------------------------------------------------------------
float LinearizeDepth(float depth)
{
    // Convert normalized depth from [0,1] to NDC [-1,1]
    float z = depth * 2.0 - 1.0;
    // Convert NDC depth to linear depth
    return (2.0 * near * far) / (far + near - z * (far - near));
}
// ----------------------------------------------------------------------------
bool IsOutsideScreen(vec2 tex_coords)
{
    return any(lessThan(tex_coords, vec2(0.0))) || any(greaterThan(tex_coords, vec2(1.0)));
}

vec4 GetViewPositionFromUV(vec2 uv)
{
   vec3 frag_pos = texture(position_map, uv).rgb;
   vec4 view_pos = view * vec4(frag_pos, 1.0);
   return vec4(vec3(view_pos.xyz / view_pos.w), 1.0);
}
// ----------------------------------------------------------------------------
void main() {
    vec3 normal = normalize(texture(normal_map, TexCoords).rgb);
    vec3 frag_pos = texture(position_map, TexCoords).rgb;
    vec3 albedo = texture(albedo_map, TexCoords).rgb;
    
    // Check if surface is reflective
    float metallic = texture(position_map, TexCoords).a;
    if (metallic < 0.01) {
        final_reflection = vec4(0.0);
        return;
    }

    vec4 uv = vec4(0.0);
    int steps = 5;

    vec4 position_from = view * vec4(frag_pos, 1.0);
    position_from.xyz /= position_from.w;
    vec3 unit_position_from = normalize(position_from.xyz);
    vec3 pivot = normalize(reflect(unit_position_from, normal));

    vec4 position_to = position_from;

    vec4 start_view = vec4(position_from.xyz + (pivot * 0.0), 1.0);
    vec4 end_view = vec4(position_from.xyz + (pivot * max_distance), 1.0);

    vec4 start_frag = start_view;
    start_frag = proj * start_frag;
    start_frag.xyz /= start_frag.w;
    start_frag.xy = start_frag.xy * 0.5 + 0.5;
    start_frag.xy = start_frag.xy * screen_size;

    vec4 end_frag = end_view;
    end_frag = proj * end_frag;
    end_frag.xyz /= end_frag.w;
    end_frag.xy = end_frag.xy * 0.5 + 0.5;
    end_frag.xy = end_frag.xy * screen_size;

    vec2 frag = start_frag.xy;
    uv.xy = frag / screen_size;

    float delta_x = end_frag.x - start_frag.x;
    float delta_y = end_frag.y - start_frag.y;
    float use_x = abs(delta_x) >= abs(delta_y) ? 1.0 : 0.0;
    float delta = mix(abs(delta_y), abs(delta_x), use_x) * clamp(resolution, 0.0, 1.0);
    vec2 increment = vec2(delta_x, delta_y) / max(delta, 0.001);

    float search_0 = 0;
    float search_1 = 0;

    int hit_0 = 0;
    int hit_1 = 0;

    float view_distance = start_view.y;
    float depth = g_max_thickness;

    float i = 0;

    for(i = 0; i < int(delta); ++i)
    {
        frag += increment;
        uv.xy = frag / screen_size;
        position_to = GetViewPositionFromUV(uv.xy);

        search_1 = 
        mix((frag.y - start_frag.y) / delta_y,
            (frag.x - start_frag.x) / delta_x,
            use_x);

        search_1 = clamp(search_1, 0.0, 1.0);

        view_distance = (start_view.y * end_view.y) / mix(end_view.y, start_view.y, search_1);
        depth = view_distance - position_to.y;

        if(depth > 0 && depth < g_max_thickness)
        {
            hit_0 = 1;
            break;
        }
        else
        {
            search_0 = search_1;
        }
    }

    search_1 = search_0 + ((search_1 - search_0) * 0.5);

    
    steps *= hit_0;
    
    for(i = 0; i < steps; ++i)
    {
        frag = mix(start_frag.xy, end_frag.xy, search_1);
        uv.xy = frag / screen_size;
        position_to = GetViewPositionFromUV(uv.xy);

        view_distance = (start_view.y * end_view.y) / mix(end_view.y, start_view.y, search_1);
        depth = view_distance - position_to.y;

        if(depth > 0 && depth < g_max_thickness)
        {
            hit_1 = 1;
            search_1 = search_0 + ((search_1 - search_0) * 0.5);
        }
        else
        {
         float temp = search_1;
         search_1 = search_1 + ((search_1 - search_0) * 0.5);
         search_0 = temp;
        }
    }



    final_reflection = vec4(texture(albedo_map, uv.xy).rgb, 0.0); 
}