#version 430 core
out float final_occlusion;

in vec2 TexCoords;

uniform sampler2D ssao_texture;

void main() {
    vec2 texel_size = 1.0 / vec2(textureSize(ssao_texture, 0));

    float result = 0.0;

    for(int x = -2; x <= 2; ++x) {
        for(int y = -2; y <= 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texel_size;
            result += texture(ssao_texture, TexCoords + offset).r;
        }
    }

    final_occlusion = result / 25.0;
}