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
uniform vec3 cam_pos;
// SSR parameters
uniform float max_distance = 100.0;     // Maximum ray distance
uniform int max_steps = 50;           // Maximum ray steps 
uniform int binary_search_steps = 5;   // Binary search refinement steps
uniform float reflection_strength = 1.0;
uniform float near = 0.1;             // Near plane distance
uniform float far = 1000.0;           // Far plane distance
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
// ----------------------------------------------------------------------------
void main() {
    vec3 normal = normalize(texture(normal_map, TexCoords).rgb);
    vec3 frag_pos = texture(position_map, TexCoords).rgb;
    vec3 albedo = texture(albedo_map, TexCoords).rgb;
    float depth = texture(depth_map, TexCoords).r;
    
    // Check if surface is reflective
    float metallic = texture(position_map, TexCoords).a;
    if (metallic < 0.01) {
        final_reflection = vec4(0.0);
        return;
    }
    
    // Calculate view direction - from camera to fragment
    vec3 view_dir = normalize(cam_pos - frag_pos);
    
    // Get reflection vector
    vec3 reflect_dir = normalize(reflect(-view_dir, normal));
    
    // Initialize ray trace
    vec3 cur_pos = frag_pos;
    bool hit = false;
    vec2 hit_pixel = vec2(0.0);
    float step_size = max_distance / float(max_steps);
    
    // Ray marching for SSR
    for(int i = 0; i < max_steps; ++i)
    {
        // Get next position
        cur_pos += reflect_dir * step_size;
        
        // Get screen space coordinates
        vec2 screen_pos = WorldToScreen(cur_pos);
        
        // Check if outside screen
        if(IsOutsideScreen(screen_pos))
            break;
            
        // Get sampled depth at current screen position and linearize it
        float cur_depth = LinearizeDepth(texture(depth_map, screen_pos).r);
        
        // Get depth of current ray position
        float ray_depth = GetLinearDepth(cur_pos);
        
        // Check if ray is behind geometry (has greater depth than the sampled depth)
        if(ray_depth > cur_depth)
        {
            hit = true;
            hit_pixel = screen_pos;
            
            // Binary search for more accurate hit position
            vec3 prev_pos = cur_pos - reflect_dir * step_size;
            vec3 mid_pos;
            
            for(int j = 0; j < binary_search_steps; ++j)
            {
                mid_pos = mix(prev_pos, cur_pos, 0.5);
                vec2 mid_screen_pos = WorldToScreen(mid_pos);
                
                if(!IsOutsideScreen(mid_screen_pos))
                {
                    float mid_depth = LinearizeDepth(texture(depth_map, mid_screen_pos).r);
                    float mid_ray_depth = GetLinearDepth(mid_pos);
                    
                    if(mid_ray_depth > mid_depth)
                        cur_pos = mid_pos;
                    else
                        prev_pos = mid_pos;
                }
            }
            
            hit_pixel = WorldToScreen(cur_pos);
            break;
        }
    }
    
    // If hit, calculate final reflection
    if(hit && !IsOutsideScreen(hit_pixel))
    {
        // Calculate edge factor for smooth transitions
        float edge_factor = 1.0;
        if(hit_pixel.x < 0.1) edge_factor *= hit_pixel.x / 0.1;
        if(hit_pixel.y < 0.1) edge_factor *= hit_pixel.y / 0.1;
        if(hit_pixel.x > 0.9) edge_factor *= (1.0 - hit_pixel.x) / 0.1;
        if(hit_pixel.y > 0.9) edge_factor *= (1.0 - hit_pixel.y) / 0.1;
        
        // Sample albedo map
        vec3 reflected_color = texture(albedo_map, hit_pixel).rgb;
        
        // Apply reflection strength and edge factor
        final_reflection = vec4(reflected_color, edge_factor * reflection_strength * metallic);
    }
    else {
        final_reflection = vec4(0.0);
    }
}