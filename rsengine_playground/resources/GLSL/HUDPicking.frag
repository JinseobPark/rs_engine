#version 330 core
out uvec4 FragColor;

in vec2 TexCoords;

uniform uint color_id;
uniform sampler2D tex;

void main() {
    vec2 texcoord_flipy = vec2(TexCoords.x, 1.0 - TexCoords.y);
    vec4 tex_color = texture(tex, texcoord_flipy);
        if(tex_color.a < 0.1) 
    {
        FragColor = uvec4(0, 0, 0, 0);
        return;
    }
    uint result = color_id;
    FragColor = uvec4(result , 0, 0, 0);
}