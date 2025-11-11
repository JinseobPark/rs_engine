#version 430 core
out float final_occlusion;

in vec2 TexCoords;

uniform sampler2D position_map;  // position + metallic
uniform sampler2D normal_map;    // normal(TBN)
uniform sampler2D noise_texture; // noise texture


uniform vec3 samples[64];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
int kernel_size = 64;
float radius = 0.5;
float bias = 0.025;

// tile noise texture over screen based on screen dimensions divided by noise size
uniform vec2 screen_size;
const float noise_size = 4.0;
//const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0); 

uniform mat4 view;
uniform mat4 projection;

void main() {
    vec3 world_position = texture(position_map, TexCoords).rgb;
    vec3 normal = texture(normal_map, TexCoords).rgb; // TBN Texture in space 

    // get view-space position
    vec4 view_space_pos = view * vec4(world_position, 1.0);
    vec3 view_pos = view_space_pos.xyz / view_space_pos.w;

    // convert normal from TBN to view-space
    vec3 view_normal = normalize(mat3(view) * normal);

    vec2 noise_scale = screen_size / noise_size;

    // get random rotation vec3
    vec3 random_vec = texture(noise_texture, TexCoords * noise_scale).xyz * 2.0 - 1.0;

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;

    for(int i = 0; i < kernel_size; ++i)
    {
        // get sample position
        vec3 sample_pos = samples[i]; // from tangent to view-space

        vec2 rotated = vec2(
        random_vec.x * sample_pos.x - random_vec.y * sample_pos.y,
        random_vec.y * sample_pos.x + random_vec.x * sample_pos.y);

        sample_pos = vec3(rotated, sample_pos.z);
        sample_pos = view_pos + (sample_pos) * radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = projection * vec4(sample_pos, 1.0);
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // sample position texture using projected coordinates
        vec3 sample_world_pos = texture(position_map, offset.xy).rgb;
        vec4 sample_view_pos = view * vec4(sample_world_pos, 1.0);
        sample_view_pos.xyz /= sample_view_pos.w;

        //float sample_depth = texture(position_map, offset.xy).z;

        // range check & accumulate
        float range_check = smoothstep(0.0, 1.0, radius / abs(view_pos.z - sample_view_pos.z));
        occlusion += (sample_view_pos.z >= sample_pos.z + bias ? 1.0 : 0.0) * range_check;
    }
    occlusion = 1.0 - occlusion / kernel_size;

    final_occlusion = occlusion;
}