#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in uint aValue; 

uniform mat4 view;
uniform mat4 projection;
uniform float point_size;

uniform bool b_clip_invert;
uniform bool b_legend_filtering;
uniform int clipping_type; // 0 - NONE, 1 - CUBE, 2 - SPHERE, 3 - PLANE
uniform mat4 clipping_obj_transform;

uniform float legend_min = 0;
uniform float legend_max = 83000;

out VS_OUT {
    float val_color;
    vec3 world_pos;
} vs_out;

void main()
{
    // cube
    if(clipping_type == 1 && b_clip_invert) // return outside of the cube
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (!(abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 1 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if ((abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // sphere
    else if(clipping_type == 2 && b_clip_invert) // return outside of the sphere
    {
        // Get calculted sphere's radius from x-scaling.
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (length(pos.xyz) < 0.5)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 2 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (length(pos.xyz) > 0.5)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // plane. If the point is on the positive side of the plane, return it.
    else if(clipping_type == 3 && b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        // the origin plane's normal vector is (0, 1, 0). If the dot product of the point and the normal vector is positive, the point is on the positive side of the plane.
        if (dot(pos, vec4(0.0, 1.0, 0.0, 0.0)) > 0.0)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 3 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (dot(pos, vec4(0.0, 1.0, 0.0, 0.0)) < 0.0)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }


    float f_value = uintBitsToFloat(aValue);

    // if the value is out of the legend range, then the particle is not drawn.
    if(b_legend_filtering && (f_value < legend_min || f_value > legend_max))
    {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        return;
    }
    
    vs_out.val_color = f_value;
    vs_out.world_pos = aPos;

    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = point_size / gl_Position.w;
} 