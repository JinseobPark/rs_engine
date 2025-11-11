#version 460

uniform sampler2D tex0;
uniform vec3 value_min;
uniform vec3 value_max;

in vec4 out_color;

out vec4 FragColor;


void main()
{   
    vec2 pointUV = gl_PointCoord;
    if(length(pointUV - vec2(0.5, 0.5)) > 0.5) 
        discard;

    vec4 tex1_color = texture(tex0, pointUV);
//    if(tex1_color.a < 0.99)
//        discard;

    vec4 value_color = out_color;
    // value_color should changed 0~1 by value_min~value_max for xyz
    vec3 min_max_diff = value_max - value_min;

    // When any element of diff is 0, then the element should be 0.1.
    min_max_diff = max(min_max_diff, vec3(0.1, 0.1, 0.1));

    value_color.rgb = (value_color.rgb - value_min) / min_max_diff;

//    // if any the value is out of the value_min and value_max, then the color should be black.
//    if(value_color.r < 0.0 || value_color.r > 1.0 || value_color.g < 0.0 || value_color.g > 1.0 || value_color.b < 0.0 || value_color.b > 1.0)
//    {
//        value_color = vec4(0.2, 0.2, 0.2, 0.3);
//    }


    FragColor = value_color * tex1_color;

//    FragColor = out_color;
}  