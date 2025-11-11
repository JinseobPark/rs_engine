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
uniform vec2 screen_size;			 // Screen size
uniform float near = 0.1;             // Near plane distance
uniform float far = 1000.0;           // Far plane distance
// SSR parameters
uniform float g_max_thickness = 1.0;
uniform float g_depthbias = 0.1;
uniform float max_distance = 50.0;     // Maximum ray distance
uniform int max_steps = 20;           // Maximum ray steps 
uniform int binary_search_steps = 5;   // Binary search refinement steps
uniform float reflection_strength = 1.0;
// Constants
const int MAX_STEPS = 256;
const float MAX_DISTANCE = 50.0;
const float STEP_SIZE = 0.1;
const float THICKNESS = 0.05;
const float EDGE_FADE_START = 0.8; // Start fading when this close to screen edge
const float EDGE_FADE_END = 0.95;  // Completely fade out at this point
const float ROUGHNESS_FACTOR = 1.0;
const float  DEPTH_BIAS = 0.01;
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
vec3 ViewToScreen(vec3 viewPos) {
    vec4 clipSpace = proj * vec4(viewPos, 1.0);
    return clipSpace.xyz / clipSpace.w;
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
// V2: Linearize depth
float linearizeDepth(float depth) {
    return near * far / (far + depth * (near - far));
}
// ----------------------------------------------------------------------------
bool IsOutsideScreen(vec2 tex_coords)
{
    return any(lessThan(tex_coords, vec2(0.0))) || any(greaterThan(tex_coords, vec2(1.0)));
}
// ----------------------------------------------------------------------------
float GetEdgeFade(vec2 screen_pos)
{
    vec2 distFromEdge = min(screen_pos, 1.0 - screen_pos);
    float minDist = min(distFromEdge.x, distFromEdge.y);
    return smoothstep(0.0, EDGE_FADE_START - EDGE_FADE_END, minDist - EDGE_FADE_END);
}
// ----------------------------------------------------------------------------
// Trace screen space ray and return hit information
bool TraceScreenSpaceRay(vec3 rayDir, vec3 viewPos, out vec3 hitPixel_alpha) {
    // Calculate ray length based on near plane distance
    float rayLength = (viewPos.z + rayDir.z * MAX_DISTANCE) < near ? 
                      (near - viewPos.z) / rayDir.z : MAX_DISTANCE;
    
    // Initialize hit pixel
    hitPixel_alpha = vec3(-1.0, -1.0, 1.0);
    
    // Calculate ray end point in view space
    vec3 rayEnd = viewPos + rayDir * rayLength;
    
    // Transform into clip space
    vec4 ssRayBegin = proj * vec4(viewPos, 1.0);
    vec4 ssRayEnd = proj * vec4(rayEnd, 1.0);
    
    // Perspective division
    float k0 = 1.0 / ssRayBegin.w;
    float k1 = 1.0 / ssRayEnd.w;
    
    // Transform to NDC space and then to screen space
    vec2 P0 = ssRayBegin.xy * k0;
    vec2 P1 = ssRayEnd.xy * k1;
    
    // Convert to screen coordinates
    P0 = (P0 * 0.5 + 0.5) * screen_size;
    P1 = (P1 * 0.5 + 0.5) * screen_size;
    
    // Prevent zero-length rays
    P1 += (dot(P1 - P0, P1 - P0) < 0.0001) ? vec2(0.01, 0.01) : vec2(0.0, 0.0);
    
    // Calculate delta for DDA
    vec2 delta = P1 - P0;
    
    // Determine if we need to swap x and y (if |dy| > |dx|)
    bool permute = false;
    if (abs(delta.x) < abs(delta.y)) {
        permute = true;
        delta = delta.yx;
        P0 = P0.yx;
        P1 = P1.yx;
    }
    
    // Determine step direction and calculate inverse dx
    float stepDir = sign(delta.x);
    float invdx = stepDir / delta.x;
    
    // Initialize counters and depth values
    float stepCount = 0.0;
    float rayZ = viewPos.z;
    float sceneZMax = viewPos.z + 100.0;
    
    // Calculate the end point for DDA
    float end = stepDir * P1.x;
    
    // Initialize current position and step increment
    vec3 Pk = vec3(P0, k0);
    vec3 dPk = vec3(stepDir, delta.y * invdx, (k1 - k0) * invdx);
    
    // Apply ray step scale
    dPk *= STEP_SIZE;
    
    // Thickness for depth comparison
    float thickness = THICKNESS;
    
    // Ray marching loop
    for (;
        ((Pk.x * stepDir) <= end) &&            // Check if we've reached the end
        (stepCount < float(MAX_STEPS)) &&   // Check if we've hit max steps
        ((rayZ < sceneZMax) || ((rayZ - thickness) > sceneZMax)) && // Check if we've hit an object
        (sceneZMax != 0.0);                      // Ensure valid depth
        Pk += dPk, stepCount += 1.0) {
        
        // Calculate hit pixel coordinates
        hitPixel_alpha.xy = permute ? Pk.yx : Pk.xy;
        hitPixel_alpha.xy /= screen_size;
        hitPixel_alpha.y = 1.0 - hitPixel_alpha.y;  // Flip Y for OpenGL
        
        // Calculate ray depth
        rayZ = 1.0 / Pk.z;
        
        // Sample depth from depth buffer
        sceneZMax = texture(depth_map, hitPixel_alpha.xy).r;
        
        // Convert normalized depth to view space depth
        sceneZMax = sceneZMax * 2.0 - 1.0; // Convert from [0,1] to [-1,1]
        vec4 clipPos = vec4(hitPixel_alpha.xy * 2.0 - 1.0, sceneZMax, 1.0);
        mat4 invProjectionMatrix = inverse(proj);
        vec4 viewPos = invProjectionMatrix * clipPos;
        sceneZMax = viewPos.z / viewPos.w;
        
        // Apply depth bias
        sceneZMax += DEPTH_BIAS;
    }
    
    // Calculate edge fade to avoid reflection artifacts at screen edges
    float edgeFade = 1.0 - pow(length(hitPixel_alpha.xy - 0.5) * 2.0, 3.0);
    hitPixel_alpha.z = edgeFade;
    
    // Return true if ray hit something
    return (rayZ >= sceneZMax) && (rayZ - thickness <= sceneZMax);
}

bool JS_TraceScreenSpaceRay(vec3 ray_dir, vec3 view_pos, inout vec3 hit_pixel_alpha) {
    // Calculate ray length based on near plane distance
    float ray_length = (view_pos.z + ray_dir.z * MAX_DISTANCE) < near ? (near - view_pos.z) / ray_dir.z : MAX_DISTANCE;
    vec2 inverse_screen_size = 1.0 / screen_size;
    hit_pixel_alpha = vec3(-1.0, -1.0, 1.0);

    vec3 ray_end = view_pos + ray_dir * ray_length;

    // Transform into clip space (screen space)
    vec4 ss_ray_begin = proj * vec4(view_pos, 1.0);
    vec4 ss_ray_end = proj * vec4(ray_end, 1.0);

    // Convert to NDC space
    float k0 = 1.0 / ss_ray_begin.w;
    float k1 = 1.0 / ss_ray_end.w;

    vec2 P0 = ss_ray_begin.xy * k0;
    vec2 P1 = ss_ray_end.xy * k1;

    P0 = (P0 * 0.5 + 0.5) * screen_size;
    P1 = (P1 * 0.5 + 0.5) * screen_size;

    // Prevent zero-length rays
    P1 += (dot(P1 - P0, P1 - P0) < 0.0001) ? vec2(0.01, 0.01) : vec2(0.0, 0.0);

    vec2 delta = P1 - P0;

    // Determine if we need to swap x and y (if |dy| > |dx|)
    bool permute = false;
    if(abs(delta.x) < abs(delta.y)) {
        permute = true;
        delta = delta.yx;
        P0 = P0.yx;
        P1 = P1.yx;
    }

    // Determine step direction and calculate inverse dx
    float step_dir = sign(delta.x);
    float inv_dx = step_dir / delta.x;

    float step_count = 0;
    float ray_z = view_pos.z;
    float scene_z_max = view_pos.z + 100.0;

    float end = step_dir * P1.x;

    vec4 Pk = vec4(P0, k0, 1.0);
    vec4 dPk = vec4(vec2(step_dir, delta.y * inv_dx), (k1 - k0) * inv_dx, 0.0);

    dPk *= STEP_SIZE;

    float thickness = g_max_thickness;

    // Ray marching loop
    for(;
        ((Pk.x * step_dir) <= end) &&
        (step_count < float(MAX_STEPS)) &&
        ((ray_z < scene_z_max) || ((ray_z - thickness) > scene_z_max)) &&
        (scene_z_max != 0.0);
        Pk += dPk, 
        step_count += 1.0) 
    {
        hit_pixel_alpha.xy = permute ? Pk.yx : Pk.xy;
        hit_pixel_alpha.xy /= inverse_screen_size;
        hit_pixel_alpha.y = 1.0 - hit_pixel_alpha.y;

        ray_z = 1.0 / Pk.w;
        scene_z_max = texture(depth_map, hit_pixel_alpha.xy).r;
        thickness = texture(depth_map, hit_pixel_alpha.xy).r * far;
        scene_z_max *= far;
        thickness -= scene_z_max;
        scene_z_max += DEPTH_BIAS;
    }

    float edge_fade = 1.0 - pow(length(hit_pixel_alpha.xy - 0.5) * 2.0, 3.0);
    hit_pixel_alpha.z = edge_fade;

    return (ray_z >= scene_z_max) && (ray_z - thickness <= scene_z_max);

}
// ----------------------------------------------------------------------------
void main() {
    vec3 normal = normalize(texture(normal_map, TexCoords).rgb);
    vec3 world_pos = texture(position_map, TexCoords).rgb;
    vec3 albedo = texture(albedo_map, TexCoords).rgb;
    float depth = texture(depth_map, TexCoords).r;
    // Check if surface is reflective
    float metallic = texture(position_map, TexCoords).a;
    if (metallic < 0.01) {
        final_reflection = vec4(0.0);
        return;
    }

    // Calculate view space position and normal
    vec3 incident_vec = normalize(mat3(view) * world_pos);
    vec3 view_normal = normalize(mat3(view) * normal);

    vec3 reflect_vec = reflect(incident_vec, view_normal);
    reflect_vec = normalize(reflect_vec);
    
    vec3 hitPixel_alpha;
    bool isHit = JS_TraceScreenSpaceRay(reflect_vec, incident_vec, hitPixel_alpha);
    
    vec3 reflectionColor = texture(albedo_map, hitPixel_alpha.xy).rgb;

    final_reflection = vec4(reflectionColor, isHit ? 1.0 : 1.0);
}