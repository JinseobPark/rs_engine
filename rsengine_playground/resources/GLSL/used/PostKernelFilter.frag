#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D input_texture;

// UBO for kernel data (std140 layout)
// std140 alignment: each array element is aligned to vec4 (16 bytes)
layout(std140, binding = 0) uniform KernelData
{
    vec4 kernel_row0;  // kernel[0], kernel[1], kernel[2], padding
    vec4 kernel_row1;  // kernel[3], kernel[4], kernel[5], padding
    vec4 kernel_row2;  // kernel[6], kernel[7], kernel[8], padding
    vec4 params;       // divisor, offset, padding, padding
};

void main()
{
    vec2 texel_size = 1.0 / textureSize(input_texture, 0);
    
    vec3 result = vec3(0.0);
    
    float divisor = params.x;
    float offset_val = params.y;
    
    // 3x3 convolution kernel application
    // Kernel layout:
    // [0][1][2]  -> kernel_row0.xyz
    // [3][4][5]  -> kernel_row1.xyz
    // [6][7][8]  -> kernel_row2.xyz
    
    // Top row
    result += texture(input_texture, TexCoords + vec2(-texel_size.x, -texel_size.y)).rgb * kernel_row0.x;
    result += texture(input_texture, TexCoords + vec2(0.0,           -texel_size.y)).rgb * kernel_row0.y;
    result += texture(input_texture, TexCoords + vec2( texel_size.x, -texel_size.y)).rgb * kernel_row0.z;
    
    // Middle row
    result += texture(input_texture, TexCoords + vec2(-texel_size.x, 0.0)).rgb * kernel_row1.x;
    result += texture(input_texture, TexCoords + vec2(0.0,           0.0)).rgb * kernel_row1.y;
    result += texture(input_texture, TexCoords + vec2( texel_size.x, 0.0)).rgb * kernel_row1.z;
    
    // Bottom row
    result += texture(input_texture, TexCoords + vec2(-texel_size.x, texel_size.y)).rgb * kernel_row2.x;
    result += texture(input_texture, TexCoords + vec2(0.0,           texel_size.y)).rgb * kernel_row2.y;
    result += texture(input_texture, TexCoords + vec2( texel_size.x, texel_size.y)).rgb * kernel_row2.z;
    
    // Apply divisor and offset
    result = result / divisor + offset_val;
    
    // Clamp result to valid range
    result = clamp(result, 0.0, 1.0);
    
    FragColor = vec4(result, 1.0);
}
