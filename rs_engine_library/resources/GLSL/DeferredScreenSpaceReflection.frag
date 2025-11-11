#version 430 core
out vec4 final_reflection;
in vec2 TexCoords;
uniform sampler2D position_map;  // position + metallic
uniform sampler2D normal_map;    // normal(TBN)
uniform sampler2D albedo_map;    // albedo + ambient
uniform sampler2D fluid_position_map; // position + metallic
uniform sampler2D fluid_normal_map;   // normal + roughness
uniform sampler2D fluid_albedo_map;   // albedo + ambient
uniform sampler2D custom_depth_map;   // r : opaque depth, g : fluid depth
uniform usampler2D custom_mask_map;   // CUSTOM_MASK_FLUID = 200
uniform sampler2D depth_map;     // depth map

// Camera uniforms
uniform mat4 view;
uniform mat4 proj;
uniform vec3 cam_pos;
// SSR parameters
uniform float max_distance = 10.0;     // Maximum ray distance
uniform int max_steps = 100;           // Maximum ray steps 
uniform float step_size = 0.2;
uniform int binary_search_steps = 10;   // Binary search refinement steps
uniform float reflection_strength = 1.0;
uniform float near = 0.1;             // Near plane distance
uniform float far = 1000.0;           // Far plane distance

uniform float g_max_thickness = 0.1;
uniform float g_depthbias = 0.01;

uniform bool use_fluid_map = false;
const uint CUSTOM_MASK_FLUID = 200;
// ----------------------------------------------------------------------------
//vec2 WorldToScreen(vec3 world_pos)
//{
//    // Convert to clip space
//    vec4 clip_space_pos = proj * view * vec4(world_pos, 1.0);
//    // Perspective division to get NDC coordinates
//    vec2 ndc = clip_space_pos.xy / clip_space_pos.w;
//    // Convert from [-1,1] to [0,1] range
//    return ndc * 0.5 + 0.5;
//}
// ----------------------------------------------------------------------------
vec2 ViewToScreen(vec3 view_pos)
{
    // Convert to clip space
    vec4 clip_space_pos = proj * vec4(view_pos, 1.0);
    // Perspective division to get NDC coordinates
    vec2 ndc = clip_space_pos.xy / clip_space_pos.w;
    // Convert from [-1,1] to [0,1] range
    return ndc * 0.5 + 0.5;
}
// ----------------------------------------------------------------------------
//float GetLinearDepth(vec3 world_pos)
//{
//    vec4 clip_space_pos = proj * view * vec4(world_pos, 1.0);
//    float ndc_depth = clip_space_pos.z / clip_space_pos.w;
//    // Convert NDC depth to linear depth
//    return (2.0 * near * far) / (far + near - ndc_depth * (far - near));
//}
// ----------------------------------------------------------------------------
//float LinearizeDepth(float depth)
//{
//    // Convert normalized depth from [0,1] to NDC [-1,1]
//    float z = depth * 2.0 - 1.0;
//    // Convert NDC depth to linear depth
//    return (2.0 * near * far) / (far + near - z * (far - near));
//}
//float linearizeDepth (float zsample) {
//	return -far * near / (zsample * (far - near) - far);
//}

//float CalculateViewDepthFromDepthMap(float depth)
//{
//    /*
//        float viewZ = nearPlane + normalizedDepth * (farPlane - nearPlane);
//        return -viewZ;
//    */
//    float viewZ = near + depth * (far - near);
//    return -viewZ;
//}

// ----------------------------------------------------------------------------
bool IsOutsideScreen(vec2 tex_coords)
{
    return any(lessThan(tex_coords, vec2(0.0))) || any(greaterThan(tex_coords, vec2(1.0)));
}

bool IsOutsideScreenFromView(vec3 view_pos)
{
    // If view_pos'z is negative, it's behind the camera
    if (view_pos.z > 0.0) return true;

    // Convert to clip space
    vec4 clip_space_pos = proj * vec4(view_pos, 1.0);
    // Perspective division to get NDC coordinates
    vec2 ndc = clip_space_pos.xy / clip_space_pos.w;
    // Convert from [-1,1] to [0,1] range
    vec2 screen_coords = ndc * 0.5 + 0.5;
    // Check if outside screen
    return any(lessThan(screen_coords, vec2(0.0))) || any(greaterThan(screen_coords, vec2(1.0)));

}

float GetScreenEdgeFactor(vec2 screen_pos)
{
  /*
    vec2 dCoords = abs(vec2(0.5, 0.5) - coords.xy);
    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
  */
  vec2 dCoords = abs(vec2(0.5, 0.5) - screen_pos);
  float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
  return screenEdgefactor;
}

//vec3 GetViewPosFromDepthMap(vec2 tex_coords)
//{
//    // Get depth value from depth map
//    float depth = texture(depth_map, tex_coords).r;
//    // Convert to view space position
//    vec4 clip_space_pos = vec4(tex_coords * 2.0 - 1.0, depth, 1.0);
//    vec4 view_space_pos = inverse(proj) * clip_space_pos;
//    view_space_pos /= view_space_pos.w;
//    return view_space_pos.xyz;
//}
// ----------------------------------------------------------------------------
void main() {

    // 0.0 - opaque, 1.0 - fluid
    float opaque_fluid_balancing = (use_fluid_map && texture(custom_mask_map, TexCoords).r == CUSTOM_MASK_FLUID) ? 1.0 : 0.0; 
    // Get normal, position, albedo and depth from the maps

    vec3 normal = mix(texture(normal_map, TexCoords).rgb, texture(fluid_normal_map, TexCoords).rgb, opaque_fluid_balancing);
    vec3 frag_pos = mix(texture(position_map, TexCoords).rgb, texture(fluid_position_map, TexCoords).rgb, opaque_fluid_balancing);
    vec3 albedo = mix(texture(albedo_map, TexCoords).rgb, texture(fluid_albedo_map, TexCoords).rgb, opaque_fluid_balancing);
    float depth = mix(texture(depth_map, TexCoords).r, texture(custom_depth_map, TexCoords).g, opaque_fluid_balancing);
//    vec3 normal = normalize(texture(normal_map, TexCoords).rgb);
//    vec3 frag_pos = texture(position_map, TexCoords).rgb;
//    vec3 albedo = texture(albedo_map, TexCoords).rgb;
//    float depth = texture(depth_map, TexCoords).r;
    
    // Check if surface is reflective
//    float metallic = texture(position_map, TexCoords).a;
    float metallic = mix(texture(normal_map, TexCoords).a, texture(fluid_normal_map, TexCoords).a, opaque_fluid_balancing);
    if (metallic < 0.01) {
        final_reflection = vec4(0.0);
        return;
    }
    
    // Convert to view pos from world pos
    vec4 view_pos = view * vec4(frag_pos, 1.0);
    view_pos /= view_pos.w;
    // Calculate view direction 
    vec3 view_dir = normalize(view_pos.xyz);

    vec4 view_normal = view * vec4(normal, 0.0);

    // Get reflection vector
    vec3 reflect_dir = normalize(reflect(view_dir, view_normal.xyz));
    
    
    float step_size = max_distance / float(max_steps);

    // The calculation run on view space.
    // The ray is casted from the fragment position in the direction of the reflection vector.

    // Initialize ray trace
    vec3 cur_pos = view_pos.xyz;
    bool hit = false;
    vec2 hit_pixel = vec2(0.0);
    
    vec2 screen_pos;
    // Ray marching for SSR
    for(int i = 0; i < max_steps; ++i)
    {
        // Get next position
        cur_pos += reflect_dir * step_size;
        
        // Get screen space coordinates
        screen_pos = ViewToScreen(cur_pos);
        
        // Check if outside screen
        if(IsOutsideScreenFromView(cur_pos))
            break;
            
        // Get sampled depth at current screen position and linearize it
        uint custom_screen_pos_index = texture(custom_mask_map, screen_pos).r; // mask + nothing
//        float cur_depth = LinearizeDepth(texture(depth_map, screen_pos).r); // view space depth
        float cur_depth = (custom_screen_pos_index == CUSTOM_MASK_FLUID) ? texture(custom_depth_map, screen_pos).g : texture(custom_depth_map, screen_pos).r;

        // Get depth of current ray position
        float ray_depth = -cur_pos.z;
        float diff = ray_depth - cur_depth;

        if(diff > g_depthbias  && diff < g_max_thickness)
        {
          hit = true;
          hit_pixel = ViewToScreen(cur_pos);

          // Binary search for more accurate hit position
          vec3 prev_pos = cur_pos - reflect_dir * step_size;
          vec3 mid_pos = cur_pos;
          for(int j = 0; j < binary_search_steps; ++j)
          {
              mid_pos = mix(prev_pos, cur_pos, 0.5);

              if(!IsOutsideScreenFromView(mid_pos))
              {
//                float mid_depth = LinearizeDepth(texture(depth_map, ViewToScreen(mid_pos)).r);

                uint custom_view_to_screen_pos_index = texture(custom_mask_map, ViewToScreen(mid_pos)).r; // mask + nothing
                float mid_depth = (custom_view_to_screen_pos_index == CUSTOM_MASK_FLUID) ? texture(custom_depth_map, ViewToScreen(mid_pos)).g : texture(custom_depth_map, ViewToScreen(mid_pos)).r;
                float mid_ray_depth = -mid_pos.z;

                if(mid_ray_depth > mid_depth)
                    cur_pos = mid_pos;
                else
                    prev_pos = mid_pos;
              }
          }
         hit_pixel = ViewToScreen(mid_pos);
         break;
        }
    }

    // If hit, calculate final reflection
    if(hit && !IsOutsideScreen(hit_pixel))
    {
//      vec3 reflected_color = texture(albedo_map, hit_pixel).rgb;
      uint hit_mask = texture(custom_mask_map, hit_pixel).r; // mask + nothing
      vec3 reflected_color = (hit_mask == CUSTOM_MASK_FLUID) ? texture(fluid_albedo_map, hit_pixel).rgb : texture(albedo_map, hit_pixel).rgb;
      final_reflection = vec4(reflected_color, GetScreenEdgeFactor(hit_pixel) * reflection_strength * metallic);
      return;
    }

    final_reflection = vec4(0.0);
}