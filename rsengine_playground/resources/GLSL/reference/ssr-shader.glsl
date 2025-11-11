#version 450 core

// Input uniforms
uniform sampler2D uPositionTexture;    // World space position
uniform sampler2D uNormalTexture;      // World space normals 
uniform sampler2D uSceneColorTexture;  // Scene color for sampling reflections
uniform sampler2D uDepthTexture;       // Linear depth values

// Camera uniforms
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uInvViewMatrix;
uniform mat4 uInvProjectionMatrix;
uniform vec3 uCameraPosition;
uniform vec2 uScreenSize;

// SSR parameters
uniform float uMaxDistance = 5.0;     // Maximum ray distance
uniform int uMaxSteps = 50;           // Maximum ray steps 
uniform int uBinarySearchSteps = 5;   // Binary search refinement steps
uniform float uReflectionStrength = 1.0;

// Input vertex attributes
in vec2 vTexCoord;

// Output
layout(location = 0) out vec4 FragColor;

// Utility functions
vec3 WorldPosFromDepth(float depth, vec2 texCoord) {
    // Convert screen space coordinates to NDC
    vec4 clipSpacePos = vec4(texCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    
    // Convert to view space
    vec4 viewSpacePos = uInvProjectionMatrix * clipSpacePos;
    viewSpacePos /= viewSpacePos.w;
    
    // Convert to world space
    vec4 worldSpacePos = uInvViewMatrix * viewSpacePos;
    
    return worldSpacePos.xyz;
}

vec2 WorldToScreen(vec3 worldPos) {
    // Convert world position to clip space
    vec4 clipPos = uProjectionMatrix * uViewMatrix * vec4(worldPos, 1.0);
    
    // Perspective division
    vec2 ndc = clipPos.xy / clipPos.w;
    
    // Convert to UV coordinates
    return ndc * 0.5 + 0.5;
}

// Calculate Fresnel effect using Schlick's approximation
float Fresnel(vec3 viewDir, vec3 normal) {
    float F0 = 0.04; // Typical value for non-metals
    return F0 + (1.0 - F0) * pow(1.0 - max(dot(viewDir, normal), 0.0), 5.0);
}

bool IsOutsideScreen(vec2 coord) {
    return coord.x < 0.0 || coord.x > 1.0 || coord.y < 0.0 || coord.y > 1.0;
}

// Main shader function
void main() {
    // Sample input textures
    vec3 worldPos = texture(uPositionTexture, vTexCoord).rgb;
    vec3 normal = normalize(texture(uNormalTexture, vTexCoord).rgb);
    float depth = texture(uDepthTexture, vTexCoord).r;
    
    // Calculate view direction
    vec3 viewDir = normalize(uCameraPosition - worldPos);
    
    // Get reflection direction
    vec3 reflectionDir = normalize(reflect(-viewDir, normal));
    
    // Calculate Fresnel factor for reflection strength
    float fresnel = Fresnel(viewDir, normal);
    
    // Initialize ray tracing
    vec3 currentPos = worldPos;
    bool hit = false;
    vec2 hitPixel = vec2(0.0);
    float stepSize = uMaxDistance / float(uMaxSteps);
    
    // Ray marching for screen space reflections
    for (int i = 0; i < uMaxSteps; i++) {
        // Step along ray
        currentPos += reflectionDir * stepSize;
        
        // Convert to screen coordinates
        vec2 screenCoord = WorldToScreen(currentPos);
        
        // Check if we're outside the screen
        if (IsOutsideScreen(screenCoord)) {
            break;
        }
        
        // Sample depth at current position
        float sampledDepth = texture(uDepthTexture, screenCoord).r;
        //vec3 sampledWorldPos = WorldPosFromDepth(sampledDepth, screenCoord);
        
        // Calculate depth difference
        float currentPosDepth = WorldToScreen(currentPos).y;
        float depthDiff = currentPosDepth - sampledDepth;
        
        // If we crossed the depth buffer, we have a hit
        if (depthDiff >= 0.0 && depthDiff < 0.1) {
            hit = true;
            
            // Binary search to refine hit position
            vec3 prevPos = currentPos - reflectionDir * stepSize;
            for (int j = 0; j < uBinarySearchSteps; j++) {
                vec3 midPos = mix(prevPos, currentPos, 0.5);
                vec2 midCoord = WorldToScreen(midPos);
                
                if (!IsOutsideScreen(midCoord)) {
                    float midDepth = texture(uDepthTexture, midCoord).r;
                    float midPosDepth = WorldToScreen(midPos).y;
                    
                    if (midPosDepth >= midDepth) {
                        currentPos = midPos;
                    } else {
                        prevPos = midPos;
                    }
                }
            }
            
            hitPixel = WorldToScreen(currentPos);
            break;
        }
    }
    
    // Sample reflection color if hit was found
    if (hit) {
        // Calculate edge fade to avoid artifacts near screen edges
        float edgeFactor = 1.0;
        if (hitPixel.x < 0.1) edgeFactor *= hitPixel.x / 0.1;
        if (hitPixel.x > 0.9) edgeFactor *= (1.0 - hitPixel.x) / 0.1;
        if (hitPixel.y < 0.1) edgeFactor *= hitPixel.y / 0.1;
        if (hitPixel.y > 0.9) edgeFactor *= (1.0 - hitPixel.y) / 0.1;
        
        // Sample scene color at hit position
        vec3 reflectionColor = texture(uSceneColorTexture, hitPixel).rgb;
        
        // Calculate reflection intensity based on fresnel and edge factor
        float intensity = fresnel * edgeFactor * uReflectionStrength;
        
        // Output reflection color and intensity for blending
        FragColor = vec4(reflectionColor, intensity);
    } else {
        // No hit, no reflection
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}
