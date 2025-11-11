#version 460

uniform sampler2D tex0;
uniform sampler2D tex1;

in float out_value;

out vec4 FragColor;

uniform float legend_min = 0;
uniform float legend_max = 83000;

uniform bool use_legend = false;

void main()
{   
    vec2 pointUV = gl_PointCoord;
    if(length(pointUV - vec2(0.5, 0.5)) > 0.5) 
        discard;

    vec4 tex0_color = texture(tex0, pointUV);

    //tex1 : legend texture
    float val = out_value;
    // calculate val to 0~1 (min, max)
    float val_per = clamp(1.0 - ( (val - legend_min) / (legend_max - legend_min) ), 0.005, 0.995);
    // val is 0 ~ max_val. convert to 0~1 about texture x-axis coordinate
    vec2 legend_color = vec2(val_per, val_per);

//    // if the value is out of the legend range, then the color should be black.
//    if(val < legend_min || val > legend_max)
//    {
//        FragColor = vec4(0.2, 0.2, 0.2, 0.3);
//        return;
//    }

    vec4 tex1_color = texture(tex1, legend_color);

    FragColor = tex0_color * tex1_color;

}  