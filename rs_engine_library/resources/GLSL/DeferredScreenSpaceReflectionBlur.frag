#version 430 core
out vec4 final_reflection;

in vec2 TexCoords;

uniform sampler2D ssr_texture;
uniform sampler2D normal_map; // normal + roughness.
uniform sampler2D fluid_normal_map; // normal + roughness.
uniform usampler2D custom_mask_map; // CUSTOM_MASK_FLUID = 200

uniform float blur_coefficient = 1.0;
uniform bool use_fluid_map = false;
const uint CUSTOM_MASK_FLUID = 200;

void main() {
    // 0.0 - opaque, 1.0 - fluid
    float opaque_fluid_balancing = (use_fluid_map && texture(custom_mask_map, TexCoords).r == CUSTOM_MASK_FLUID) ? 1.0 : 0.0; 

	// This blur is used to Gaussian 3x3 blur the reflection.
	// The blur kernel function based on the url https://en.wikipedia.org/wiki/Kernel_(image_processing)

	// As roughness, it will be used to blur the reflection.

	// 0 - no blur. 1 - full blur.
	vec2 pixel_size = 1.0 / textureSize(ssr_texture, 0);

//	float roughness = texture(normal_map, TexCoords).a;
	float roughness = mix(texture(normal_map, TexCoords).a, texture(fluid_normal_map, TexCoords).a, opaque_fluid_balancing); 

	// blur weight array 3x3. 
	const vec2 blur_weights[9] = vec2[](
	vec2(-1.0, -1.0),	vec2(0.0, -1.0),	vec2(1.0, -1.0),
	vec2(-1.0, 0.0),	vec2(0.0, 0.0),		vec2(1.0, 0.0),
	vec2(-1.0, 1.0),	vec2(0.0, 1.0),		vec2(1.0, 1.0)
	);

	const float gaussian_blur_weight[9] = float[](
		1, 2, 1,
		2, 4, 2,
		1, 2, 1
	);

	// Blur the reflection.
	vec4 reflection = texture(ssr_texture, TexCoords);
	vec3 blurred_reflection = vec3(0.0);
	vec2 blur_base = pixel_size * vec2(2.0);
	float gaussian_blur_weight_point_sum = 0.0f;

	for (int i = 0; i < 9; i++) {
		vec2 offset = blur_base + gaussian_blur_weight[i] * blur_coefficient * pixel_size * blur_weights[i] * roughness;
		vec3 sampled_color = texture(ssr_texture, TexCoords + offset).rgb;
		blurred_reflection += sampled_color;
		gaussian_blur_weight_point_sum += gaussian_blur_weight[i];
	}
	blurred_reflection /= gaussian_blur_weight_point_sum;
	final_reflection = vec4(blurred_reflection, reflection.a);
}
