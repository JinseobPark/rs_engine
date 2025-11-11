#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aColor; 

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float point_size;
uniform int clipping_id = 0;
uniform mat4 box_transform;

out vec4 out_color;

void main()
{
    vec4 color = vec4(aColor, 1.0);
    // if aPos is outside the box, discard the point.
    // The box is 1x1x1 centered at the origin. And the box_transform is the transformation matrix that scales and translates and rotates the box.
    if(clipping_id == 1)
    {
        vec4 pos = inverse(box_transform) * vec4(aPos, 1.0);
        if (!(abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
	    {
		    gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
		    return;
	    }
    }
    else if(clipping_id == 0)
    {
        vec4 pos = inverse(box_transform) * vec4(aPos, 1.0);
        if (abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5)
	    {
            color = vec4(0.2, 0.2, 0.2, 0.3);
	    }
    }


    out_color = color;
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = point_size / gl_Position.w;
} 