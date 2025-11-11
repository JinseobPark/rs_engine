#version 430 core

in vec3 val_color;
in vec3 world_pos;
out vec4 FragColor;


uniform mat4 view;
uniform mat4 projection;


uniform vec4 point_min_point_type = vec4(0, 0, 0, 0);
uniform vec4 vector_max_radius_inout = vec4(0, 0, 0, 1);

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform bool use_legend = false;
uniform bool use_texture_second = false;

// Should get min/max value
uniform float max_value = 10.0;
uniform float min_value = 0.0;
int clip_plane(vec3 point, vec3 normal, vec3 world_pos) {
	return (dot(world_pos - point, normal) < 0) ? 1 : -1;
}

int clip_box(vec3 min, vec3 max, vec3 world_pos) {
	return (world_pos.x < min.x || world_pos.x > max.x || world_pos.y < min.y || world_pos.y > max.y || world_pos.z < min.z || world_pos.z > max.z) ? -1 : 1;
}

int clip_sphere(vec3 center, float radius, vec3 world_pos) {
	return (length(world_pos - center) < radius) ? 1 : -1;
}


void main() {
    // calculate clipping
    switch (int(point_min_point_type.w)) {
        // plane clipping case
		case 1:
            // calculate that world position is above the plane which is defined with 3d point and 3d vector.
            // position = point_min_point_type.xyz
            // vector = vector_max_radius_inout.xyz
            if(clip_plane(point_min_point_type.xyz, vector_max_radius_inout.xyz, world_pos) * vector_max_radius_inout.w < 0) {
				discard;
			}
			break;

            // cube min max clipping case
		case 2:
            // calculate that world position is inside the box which is defined with min point and max point.
            // min = point_min_point_type.xyz
            // max = vector_max_radius_inout.xyz
            if(clip_box(point_min_point_type.xyz, vector_max_radius_inout.xyz, world_pos) * vector_max_radius_inout.w < 0) {
				discard;
			}
			break;

            // sphere clipping case
		case 3:
            // calculate that world position is inside the sphere which is defined with center point and radius.
            // center = point_min_point_type.xyz
            // radius = vector_max_radius_inout.x
            if(clip_sphere(point_min_point_type.xyz, vector_max_radius_inout.x, world_pos) * vector_max_radius_inout.w < 0) {
				discard;
			}
			break;
        default : 
            break;
	}



    // calculate legend texture color
    if(use_legend) {
    // if out of circle, discard
        vec2 pointUV = gl_PointCoord;
        if(length(pointUV - vec2(0.5, 0.5)) > 0.5) 
            discard;

        float val = length(val_color);
        // calculate val to 0~1 (min, max)
        float val_per = clamp(1.0 - ((val - min_value) / (max_value - min_value)), 0.005, 0.995);
        // val is 0 ~ max_val. convert to 0~1 about texture x-axis coordinate
        vec2 legend_color = vec2(0.5, val_per);
        //vec4 tex0_color = texture(tex0, legend_color);
        vec4 tex0_color = texture(tex0, legend_color);

        vec4 tex1_color = vec4(1.0, 1.0, 1.0, 1.0);

        if(use_texture_second)
        {
            tex1_color = texture(tex1, pointUV);

            if(tex1_color.a < 0.1) discard;

        }
        FragColor = tex0_color * tex1_color;
//        FragColor.a = 0.1;
        //FragColor = tex0_color;
       return;
    }
    else{
        //  val_color is -10 ~ 10. convert to 0~1
        vec2 pointUV = gl_PointCoord;
        if(length(pointUV - vec2(0.5, 0.5)) > 0.5) 
            discard;
        vec3 satured_color = val_color * 0.05 + 0.5;
    //    vec3 satured_color = val_color;
    //    satured_color = abs(satured_color.xyz);
        satured_color.x += 0.1;

        // 

        satured_color.z += 0.2;
        FragColor = vec4(satured_color.yxz + 0.2, 1.0);
    //    FragColor = vec4(satured_color.xyz, 1.0);
    //    FragColor = vec4(0, 0, 1, 1.0);
    }
}