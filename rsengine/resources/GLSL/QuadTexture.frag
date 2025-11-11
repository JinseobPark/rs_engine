#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D quad_texture;

void main() {
    vec4 quad_color = texture(quad_texture, TexCoords);
        if(quad_color.a < 0.1) discard;
    FragColor = quad_color;  // Simple direct texturing, blend in application
}