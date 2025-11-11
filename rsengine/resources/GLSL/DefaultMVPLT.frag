#version 460
out vec4 FragColor;

in float _diffuse;
in vec2 TexCoords;
uniform vec3 color;
uniform sampler2D tex;
void main()
{
    float ambient = 0.5f;
    vec2 texcoord_flipy = vec2(TexCoords.x, 1.0 - TexCoords.y);
    vec4 tex_color = texture(tex, texcoord_flipy);
            if(tex_color.a < 0.1) 
            discard;
    
    FragColor = clamp(ambient + _diffuse*0.25f, 0.0, 1.0) * tex_color * vec4(color, 1.0f);
}