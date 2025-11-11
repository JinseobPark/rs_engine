#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform usampler2D quad_r32_texture;

void main() {
    uint data = texture(quad_r32_texture, TexCoords).r; // Retrieve the integer data
    if(data == 0u) // If the data is 0, it means the pixel is empty
    {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // Grayscale representation
		discard;
    }
    float intensity = float(data) / 4294967295.0; // Normalize assuming 32-bit max value
    FragColor = vec4(intensity, 0.0, 0.0, 1.0); // Grayscale representation
}