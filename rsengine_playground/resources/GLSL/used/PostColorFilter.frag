#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D input_texture;

// UBO for color filter data (std140 layout)
// Total: 32 bytes (2 x vec4)
layout(std140, binding = 1) uniform ColorFilterData
{
    vec4 weights_intensity;  // weights.xyz, intensity
    vec4 params;             // mode, padding, padding, padding
};

void main()
{
    vec3 color = texture(input_texture, TexCoords).rgb;
    vec3 weights = weights_intensity.xyz;
    float intensity = weights_intensity.w;
    int mode = int(params.x);
    
    vec3 result = color;
    
    // Mode: 0=None, 1=Grayscale, 2=Red, 3=Green, 4=Blue, 5=Sepia, 6=Invert, 7=Custom
    if (mode == 1) // Grayscale
    {
        float gray = dot(color, weights);
        result = vec3(gray);
    }
    else if (mode == 2) // Red channel only
    {
        result = vec3(color.r, 0.0, 0.0);
    }
    else if (mode == 3) // Green channel only
    {
        result = vec3(0.0, color.g, 0.0);
    }
    else if (mode == 4) // Blue channel only
    {
        result = vec3(0.0, 0.0, color.b);
    }
    else if (mode == 5) // Sepia
    {
        float gray = dot(color, vec3(0.299, 0.587, 0.114));
        result = vec3(gray * weights.x, gray * weights.y, gray * weights.z);
    }
    else if (mode == 6) // Invert
    {
        result = vec3(1.0) - color;
    }
    else if (mode == 7) // Custom - use weights as multipliers
    {
        float gray = dot(color, vec3(0.299, 0.587, 0.114));
        result = vec3(gray) * weights;
    }
    
    // Blend between original and filtered based on intensity
    result = mix(color, result, intensity);
    
    // Clamp result
    result = clamp(result, 0.0, 1.0);
    
    FragColor = vec4(result, 1.0);
}
