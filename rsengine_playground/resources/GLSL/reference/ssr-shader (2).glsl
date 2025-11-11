#version 430 core

// Input textures
uniform sampler2D worldPositionTexture;
uniform sampler2D worldNormalTexture;
uniform sampler2D worldAlbedoTexture;
uniform sampler2D metallicTexture;
uniform sampler2D depthTexture;

// Camera matrices
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 inverseViewMatrix;
uniform mat4 inverseProjectionMatrix;
uniform vec2 screenSize;
uniform float nearPlane;
uniform float farPlane;

// Input from vertex shader
in vec2 TexCoords;

// Output
out vec4 final_reflection;

// Constants
const int MAX_STEPS = 256;
const float MAX_DISTANCE = 50.0;
const float STEP_SIZE = 0.1;
const float THICKNESS = 0.05;
const float EDGE_FADE_START = 0.8; // Start fading when this close to screen edge
const float EDGE_FADE_END = 0.95;  // Completely fade out at this point
const float ROUGHNESS_FACTOR = 1.0;

// Utility functions
float linearizeDepth(float depth) {
    return nearPlane * farPlane / (farPlane + depth * (nearPlane - farPlane));
}

vec2 getScreenCoord(vec3 worldPos) {
    // Convert world position to clip space
    vec4 clipPos = projectionMatrix * viewMatrix * vec4(worldPos, 1.0);
    clipPos.xy /= clipPos.w;
    
    // Convert to UV coordinates [0,1]
    return clipPos.xy * 0.5 + 0.5;
}

float getEdgeFade(vec2 screenPos) {
    vec2 distFromEdge = min(screenPos, 1.0 - screenPos);
    float minDist = min(distFromEdge.x, distFromEdge.y);
    return smoothstep(0.0, EDGE_FADE_START - EDGE_FADE_END, minDist - EDGE_FADE_END);
}

// DDA ray marching
bool traceScreenSpaceRay(vec3 worldPos, vec3 worldReflectDir, out vec2 hitPixel, out float confidence) {
    // Convert to view space
    vec3 viewPos = (viewMatrix * vec4(worldPos, 1.0)).xyz;
    vec3 viewDir = normalize((viewMatrix * vec4(worldReflectDir, 0.0)).xyz);
    
    // Project start and end points to screen space
    vec2 startSS = getScreenCoord(worldPos);
    vec3 viewEnd = viewPos + viewDir * MAX_DISTANCE;
    vec4 worldEnd = inverseViewMatrix * vec4(viewEnd, 1.0);
    vec2 endSS = getScreenCoord(worldEnd.xyz);
    
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
    vec2 cellSize = 1.0 / screenSize;
    vec2 rayStep = rayDir * STEP_SIZE;
    vec2 currentPos = startSS;
    
    // Slight offset to avoid self-intersection
    currentPos += rayStep * 2.0;
    
    // Start ray depth slightly in front of current position
    float startDepth = linearizeDepth(texture(depthTexture, startSS).r);
    float currentRayDepth = startDepth;
    float depthSlope = (linearizeDepth(texture(depthTexture, endSS).r) - startDepth) / deltaDist;
    
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
        float sampledDepth = linearizeDepth(texture(depthTexture, currentPos).r);
        
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
    confidence *= getEdgeFade(hitPixel);
    
    return hit;
}

void main() {
    // Sample input textures
    vec3 worldPos = texture(worldPositionTexture, TexCoords).rgb;
    vec3 worldNormal = normalize(texture(worldNormalTexture, TexCoords).rgb);
    vec4 albedo = texture(worldAlbedoTexture, TexCoords);
    float metallic = texture(metallicTexture, TexCoords).r;
    
    // Skip SSR for non-metallic surfaces
    if (metallic < 0.1) {
        final_reflection = vec4(0.0);
        return;
    }
    
    // Calculate view direction and reflection vector
    vec3 viewPos = (viewMatrix * vec4(worldPos, 1.0)).xyz;
    vec3 viewDir = normalize(viewPos);
    vec3 worldViewDir = normalize(worldPos - inverseViewMatrix[3].xyz);
    vec3 worldReflectDir = reflect(worldViewDir, worldNormal);
    
    // Trace reflection ray
    vec2 hitPixel;
    float confidence;
    bool hit = traceScreenSpaceRay(worldPos, worldReflectDir, hitPixel, confidence);
    
    // Sample color at hit location
    vec4 reflectionColor = hit ? texture(worldAlbedoTexture, hitPixel) : vec4(0.0);
    
    // Apply Fresnel effect (more reflection at grazing angles)
    float fresnel = pow(1.0 - max(0.0, dot(-worldViewDir, worldNormal)), 5.0);
    
    // Adjust reflection intensity based on metallic value
    float reflectionIntensity = mix(0.04, 1.0, metallic) * fresnel;
    
    // Apply edge fade and confidence
    reflectionIntensity *= confidence;
    
    // Output final reflection
    final_reflection = reflectionColor * reflectionIntensity;
}