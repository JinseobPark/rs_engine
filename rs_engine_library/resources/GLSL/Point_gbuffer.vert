#version 460
layout(std430, binding = 0) buffer PositionOut {
    vec4 positions[];
};

layout(std430, binding = 1) buffer VelocityOut {
    vec4 velocities[];
};

layout(std430, binding = 2) buffer AccelerationOut {
	vec4 accelerations[];
};

layout(location = 0) in uint instanceID;


out VS_OUT {
    vec3 val_color;
    vec3 world_pos;
} vs_out;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 camera_position;
// 0 - none
// 1 - velocity
// 2 - acceleration
// 3 - density (position.w)
// 4 - pressure (velocity.w)
uniform int color_type = 0;

float screen_size = 1024.0f;

void main()
{
	if(accelerations[instanceID].w == -1.0)
	{
		gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
		return;
	}

    vs_out.val_color = vec3(0.0);
    switch(color_type) {
		case 0:
			vs_out.val_color = vec3(velocities[instanceID].xyz);
			break;
		case 1:
			vs_out.val_color = vec3(velocities[instanceID].xyz);
			break;
		case 2:
			vs_out.val_color = vec3(accelerations[instanceID].xyz);
			break;
		case 3:
			vs_out.val_color = vec3(positions[instanceID].w, 0, 0);
			break;
		case 4:
			vs_out.val_color = vec3(velocities[instanceID].w, 0, 0);
			break;
		default:
			vs_out.val_color = vec3(1.0);
			break;
	}




    vec3 pos = positions[instanceID].xyz;
    gl_Position = projection * view * vec4(pos, 1.0);
    vs_out.world_pos = pos;

    float distance_from_camera = max(length(camera_position - pos), 0.001);

    //gl_Position = vec4(pos, 1.0);
    gl_PointSize = screen_size / distance_from_camera;

}

