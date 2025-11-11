#version 460
layout (location = 0) out vec4 gPosition; // position + metallic
layout (location = 1) out vec4 gNormal;   // normal + roughness
layout (location = 2) out vec4 gAlbedoSpec; // albedo + ao
layout (location = 3) out uvec4 gCustomIndex; // mask, nothing 3
//layout (location = 3) out vec4 gDepth;

in VS_OUT {
    vec3 val_color;
    vec3 world_pos;
} fs_in;

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform vec4 point_min_point_type = vec4(0, 0, 0, 0);
uniform vec4 vector_max_radius_inout = vec4(0, 0, 0, 1);

uniform bool use_legend = false;
uniform bool use_texture_second = false;

// Should get min/max value
uniform float max_value = 10.0;
uniform float min_value = 0.0;

uniform mat4 view;
uniform mat4 projection;

uniform float ambient = 0.8;

// Water INFO
// metallic = 0.0
// roughness = 0.01
// IOR = 1.333
// Specular = 0.25

//const float water_metallic = 0.0;
const float water_metallic = 0.8; // for debugging
//const float water_roughness = 0.01;
const float water_roughness = 0.2; // for debugging 
const float water_IOR = 1.333;
const float water_specular = 0.25;

const uint CUSTOM_MASK_FLUID = 200;


// ----------------------------------------------------------------------------
int clip_plane(vec3 point, vec3 normal, vec3 world_pos) {
	return (dot(world_pos - point, normal) < 0) ? 1 : -1;
}
// ----------------------------------------------------------------------------
int clip_box(vec3 min, vec3 max, vec3 world_pos) {
	return (world_pos.x < min.x || world_pos.x > max.x || world_pos.y < min.y || world_pos.y > max.y || world_pos.z < min.z || world_pos.z > max.z) ? -1 : 1;
}
// ----------------------------------------------------------------------------
int clip_sphere(vec3 center, float radius, vec3 world_pos) {
	return (length(world_pos - center) < radius) ? 1 : -1;
}
// ----------------------------------------------------------------------------
void main()
{
 // calculate clipping
 switch (int(point_min_point_type.w)) {
    // plane clipping case
	case 1:
        // calculate that world position is above the plane which is defined with 3d point and 3d vector.
        // position = point_min_point_type.xyz
        // vector = vector_max_radius_inout.xyz
        if(clip_plane(point_min_point_type.xyz, vector_max_radius_inout.xyz, fs_in.world_pos) * vector_max_radius_inout.w < 0) {
			discard;
		}
		break;

        // cube min max clipping case
	case 2:
        // calculate that world position is inside the box which is defined with min point and max point.
        // min = point_min_point_type.xyz
        // max = vector_max_radius_inout.xyz
        if(clip_box(point_min_point_type.xyz, vector_max_radius_inout.xyz, fs_in.world_pos) * vector_max_radius_inout.w < 0) {
			discard;
		}
		break;

        // sphere clipping case
	case 3:
        // calculate that world position is inside the sphere which is defined with center point and radius.
        // center = point_min_point_type.xyz
        // radius = vector_max_radius_inout.x
        if(clip_sphere(point_min_point_type.xyz, vector_max_radius_inout.x, fs_in.world_pos) * vector_max_radius_inout.w < 0) {
			discard;
		}
		break;
    default : 
        break;
    }
    // Calculate normal from point

    // if out of circle, discard
    vec2 pointUV = gl_PointCoord;

    vec3 normal;
    vec2 normal_point_uv = pointUV;
    normal_point_uv.y = 1.0 - normal_point_uv.y;
    normal.xy = normal_point_uv * 2.0 - 1.0;
    float r2 = dot(normal.xy, normal.xy);
    if (r2 > 1.0) discard;
    normal.z = (sqrt(1.0 - r2)) * 2.0 - 1.0;

    vec4 world_normal = inverse(view) * vec4(normal, 0.0);
    vec3 view_normal_dir = normalize(world_normal.xyz);
    gNormal = vec4(view_normal_dir, water_roughness);
    gPosition = vec4(fs_in.world_pos, water_metallic);

    gCustomIndex = uvec4(CUSTOM_MASK_FLUID); // nothing
//    gCustomIndex.r = CUSTOM_MASK_FLUID; // Opaque mask


    // calculate legend texture color
    if(use_legend) {

        float val = length(fs_in.val_color);
        // calculate val to 0~1 (min, max)
        float val_per = clamp(1.0 - ((val - min_value) / (max_value - min_value)), 0.005, 0.995);
        // val is 0 ~ max_val. convert to 0~1 about texture x-axis coordinate
        vec2 legend_color = vec2(val_per, val_per);
        //vec4 tex0_color = texture(tex0, legend_color);
        vec4 tex0_color = texture(tex0, legend_color);

        vec4 tex1_color = vec4(1.0, 1.0, 1.0, ambient);

        if(use_texture_second)
        {
            tex1_color = texture(tex1, pointUV);

            if(tex1_color.a < 0.1) discard;
        }
        gAlbedoSpec = tex0_color * tex1_color;
       return;
    }
    else{
    //  val_color is -10 ~ 10. convert to 0~1
    vec3 satured_color = fs_in.val_color * 0.05 + 0.5;
    //gAlbedoSpec = vec4(satured_color, 1.0f);
    gAlbedoSpec = vec4(0.824, 0.91, 0.93, ambient);

    }

}