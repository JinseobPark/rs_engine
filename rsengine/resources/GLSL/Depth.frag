#version 460
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 Position;
in vec2 TexCoords;
in vec3 Normal;

uniform vec3 color;
uniform sampler2D tex;

void main()
{
    vec2 texcoord_flipy = vec2(TexCoords.x, 1.0 - TexCoords.y);
    vec4 tex_color = texture(tex, texcoord_flipy);
            if(tex_color.a < 0.1) 
            discard;
    
    gPosition = Position;
    gNormal = normalize(Normal);
    gAlbedoSpec.xyz = color * tex_color.rgb;
    gAlbedoSpec.a = tex_color.a;
}

    #version 330 core

    uniform float near;
    uniform float far;

    float LinearizeDepth(float depth) 
    {
        float z = depth * 2.0 - 1.0; // back to NDC 
        return (2.0 * near * far) / (far + near - z * (far - near));	
    }
