#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hud_texture;
uniform vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
uniform int flipY = 1;

void main()
{
	// Because the texture is flipped vertically, we need to flip the texture coordinates
	// flipY is 1 true, -1 false. If flipY is true, we flip the texture coordinates. without if condition

	float float_flipy = float(flipY);
	vec2 Texcoord_flipy = float_flipy * vec2( TexCoords.x, 1.0 - TexCoords.y * float_flipy) + (1.0 - float_flipy) * TexCoords;

	FragColor = texture(hud_texture, Texcoord_flipy) * color;
}