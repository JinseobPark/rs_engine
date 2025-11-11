#version 430 core
out vec4 final_reflection;

in vec2 TexCoords;

uniform sampler2D position_map;  // position + metallic
uniform sampler2D normal_map;    // normal(TBN)
uniform sampler2D albedo_map;    // albedo + ambient
uniform sampler2D depth_map;     // depth map

const float ray_step = 0.25;
const float min_ray_step = 0.1;
const float max_steps = 20;
const float search_dist = 5;
const float search_dist_inv = 0.2;
const int num_binary_search_steps = 5;
const float max_d_depth = 1.0;
const float max_d_depth_inv = 1.0;

uniform mat4 projection;
uniform vec3 view_pos;



// ----------------------------------------------------------------------------
vec3 BinarySearch(vec3 dir, inout vec3 hit_coord, out float d_depth)
{
	float depth;

	for(int i = 0; i < num_binary_search_steps; ++i)
	{
		// move along the direction
		vec4 projected_coord = projection * vec4(hit_coord, 1.0);
		projected_coord.xy /= projected_coord.w;
		projected_coord.xy = projected_coord.xy * 0.5 + 0.5;

		// get depth
		depth = texture(depth_map, projected_coord.xy).z;

		// get d_depth
		d_depth = hit_coord.z - depth;

		if(d_depth > 0.0)
			hit_coord += dir;

		dir *= 0.5;
		hit_coord -= dir;
	}

	vec4 projected_coord = projection * vec4(hit_coord, 1.0);
	projected_coord.xy /= projected_coord.w;
	projected_coord.xy = projected_coord.xy * 0.5 + 0.5;

	return vec3(projected_coord.xy, depth);
}

// ----------------------------------------------------------------------------

vec4 RayCast(vec3 dir, inout vec3 hit_coord, out float d_depth)
{
	// ray casting
	dir *= ray_step;
	float depth;

	for(int i = 0; i < max_steps; ++i)
	{
		hit_coord += dir;

		vec4 projected_coord = projection * vec4(hit_coord, 1.0);
		projected_coord.xy /= projected_coord.w;
		projected_coord.xy = projected_coord.xy * 0.5 + 0.5;

		// get depth
		depth = texture(depth_map, projected_coord.xy).z;

		d_depth = hit_coord.z - depth;

		if(d_depth < 0.0)
		{
			return vec4(BinarySearch(dir, hit_coord, d_depth), 1.0);
		}
	}

  return vec4(0.0);
}

// ----------------------------------------------------------------------------

void main() {

	vec3 normal = texture(normal_map, TexCoords).rgb;
	vec3 frag_pos = texture(position_map, TexCoords).rgb;
	vec3 albedo = texture(albedo_map, TexCoords).rgb;
	
    vec3 I = normalize(frag_pos - view_pos);
    vec3 R = reflect(I, normalize(normal));
	// reflection vector
	vec3 reflected = normalize(reflect(normalize(view_pos), normalize(normal)));

	// find the reflection ray
	vec3 hit_pos = view_pos;
	float d_depth;

	vec4 coords = RayCast(R * max(min_ray_step, -view_pos.z), hit_pos, d_depth);

	vec2 d_coords = abs(vec2(0.5) - coords.xy);

	float screen_edge_factor = clamp(1.0 - (d_coords.x + d_coords.y), 0.0, 1.0);

	final_reflection = vec4(texture2D(albedo_map, coords.xy).rgb * screen_edge_factor, 1.0);
}