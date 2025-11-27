#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D input_texture;

// UBO for kernel data (std140 layout for compatibility)
layout(std140, binding = 0) uniform KernelData
{
    float kernel[12];  // 9 values + 3 padding for std140 alignment (vec4 aligned)
    float divisor;     // Normalization divisor
    float offset;      // Color offset after convolution
    float padding[2];  // Padding for alignment
};

void main()
{
    vec2 texel_size = 1.0 / textureSize(input_texture, 0);
    
    vec3 result = vec3(0.0);
    
    // 3x3 convolution kernel application
    // Kernel layout:
    // [0][1][2]
    // [3][4][5]
    // [6][7][8]
    
    // Top row
    result += texture(input_texture, TexCoords + vec2(-texel_size.x, -texel_size.y)).rgb * kernel[0];
    result += texture(input_texture, TexCoords + vec2(0.0,           -texel_size.y)).rgb * kernel[1];
    result += texture(input_texture, TexCoords + vec2( texel_size.x, -texel_size.y)).rgb * kernel[2];
    
    // Middle row
    result += texture(input_texture, TexCoords + vec2(-texel_size.x, 0.0)).rgb * kernel[3];
    result += texture(input_texture, TexCoords + vec2(0.0,           0.0)).rgb * kernel[4];
    result += texture(input_texture, TexCoords + vec2( texel_size.x, 0.0)).rgb * kernel[5];
    
    // Bottom row
    result += texture(input_texture, TexCoords + vec2(-texel_size.x, texel_size.y)).rgb * kernel[6];
    result += texture(input_texture, TexCoords + vec2(0.0,           texel_size.y)).rgb * kernel[7];
    result += texture(input_texture, TexCoords + vec2( texel_size.x, texel_size.y)).rgb * kernel[8];
    
    // Apply divisor and offset
    result = result / divisor + offset;
    
    // Clamp result to valid range
    result = clamp(result, 0.0, 1.0);
    
    FragColor = vec4(result, 1.0);
}
