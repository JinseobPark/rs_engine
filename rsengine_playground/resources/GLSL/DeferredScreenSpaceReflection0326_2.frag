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
// DDA ray marching
bool traceScreenSpaceRay(vec3 worldPos, vec3 worldReflectDir, out vec2 hitPixel, out float confidence) {
    // Convert to view space
    vec3 viewPos = (view * vec4(worldPos, 1.0)).xyz;
    vec3 viewDir = normalize((view * vec4(worldReflectDir, 0.0)).xyz);
    mat4 inverseViewMatrix = inverse(view);
    
    // Project start and end points to screen space
    vec2 startSS = WorldToScreen(worldPos);
    vec3 viewEnd = viewPos + viewDir * MAX_DISTANCE;
    vec4 worldEnd = inverseViewMatrix * vec4(viewEnd, 1.0);
    vec2 endSS = WorldToScreen(worldEnd.xyz);
    
    // Handle rays that go off-screen by finding intersection with screen edge
    bool rayGoesOffscreen = endSS.x < 0.0 || endSS.x > 1.0 || endSS.y < 0.0 || endSS.y > 1.0;
    if (rayGoesOffscreen) {
        // Find intersection with screen edges
        vec2 invDir = 1.0 / (endSS - startSS);
        
        // Calculate distances to x=0, x=1, y=0, y=1 planes
        vec4 t = vec4(
            (0.0 - startSS.x) * invDir.x,
            (1.0 - startSS.x) * invDir.x,
            (0.0 - startSS.y) * invDir.y,
            (1.0 - startSS.y) * invDir.y
        );
        
        // Find the closest intersection that's ahead of us
        vec4 tClip = max(vec4(0.0), t);
        float tMin = min(min(tClip.x, tClip.y), min(tClip.z, tClip.w));
        
        // Check if we're intersecting with any screen edge
        if (tMin < 1.0) {
            // Calculate new endpoint at screen edge
            endSS = startSS + tMin * (endSS - startSS);
        }
    }
    
    // Calculate delta and screen-space ray direction
    vec2 deltaTexCoord = endSS - startSS;
    float deltaDist = length(deltaTexCoord);
    
    // Early exit if ray points off-screen or is too short
    if (deltaDist < 0.001) {
        return false;
    }
    
    vec2 rayDir = deltaTexCoord / deltaDist;
    
    // DDA setup
    vec2 cellSize = 1.0 / screen_size;
    vec2 rayStep = rayDir * STEP_SIZE;
    vec2 currentPos = startSS;
    
    // Slight offset to avoid self-intersection
    currentPos += rayStep * 2.0;
    
    // Start ray depth slightly in front of current position
    float startDepth = linearizeDepth(texture(depth_map, startSS).r);
    float currentRayDepth = startDepth;
    float depthSlope = (linearizeDepth(texture(depth_map, endSS).r) - startDepth) / deltaDist;
    
    // DDA ray marching
    int steps = 0;
    bool hit = false;
    float bestDelta = 1.0;
    
    while (steps < MAX_STEPS && !hit) {
        // Move along ray
        currentPos += rayStep;
        steps++;
        
        // Check if we went off-screen
        if (currentPos.x < 0.0 || currentPos.x > 1.0 || currentPos.y < 0.0 || currentPos.y > 1.0) {
            break;
        }
        
        // Update ray depth
        float rayDist = length(currentPos - startSS);
        currentRayDepth = startDepth + rayDist * depthSlope;
        
        // Sample depth at current position
        float sampledDepth = linearizeDepth(texture(depth_map, currentPos).r);
        
        // Check for intersection (ray gone below surface)
        float delta = sampledDepth - currentRayDepth;
        
        if (delta < 0.0 && abs(delta) < THICKNESS && abs(delta) < bestDelta) {
            bestDelta = abs(delta);
            hitPixel = currentPos;
            hit = true;
        }
    }
    
    // Calculate confidence based on number of steps and hit quality
    confidence = hit ? (1.0 - float(steps) / float(MAX_STEPS)) * (1.0 - bestDelta / THICKNESS) : 0.0;
    confidence *= GetEdgeFade(hitPixel);
    
    return hit;
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
    
    vec3 view_pos = (view * vec4(world_pos, 1.0)).xyz;
    vec3 view_dir = normalize(view_pos); // -view_pos?
    mat4 inverse_view = inverse(view);
    vec3 world_view_dir = normalize(world_pos - inverse_view[3].xyz);
    vec3 world_reflect_dir = reflect(world_view_dir, normal);

    // Trace Reflection Ray
    vec2 hit_pixel;
    float confidence;
    bool hit = traceScreenSpaceRay(world_pos, world_reflect_dir, hit_pixel, confidence);

    // Sample color at hit location
    vec3 reflection_color = hit ? texture(albedo_map, hit_pixel).rgb : vec3(0.0);

    float fresnel = hit ? 1.0 : 0.0;

    final_reflection = vec4(reflection_color, fresnel);


}