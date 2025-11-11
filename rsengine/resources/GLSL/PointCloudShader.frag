#version 460

uniform sampler2D tex0;

in vec4 out_color;

out vec4 FragColor;

uniform int clipping_id = 0;
uniform vec3 clipping_color;

void main()
{   
//    if(clipping_id == 1){
//    	if(out_color.x > clipping_color.x || out_color.y > clipping_color.y || out_color.z > clipping_color.z) {
//		discard;
//	    }
//	}

    vec2 pointUV = gl_PointCoord;
    if(length(pointUV - vec2(0.5, 0.5)) > 0.5) 
        discard;
    vec4 tex1_color = texture(tex0, pointUV);
//    if(tex1_color.a < 0.99)
//        discard;
    FragColor = out_color * tex1_color;

//    FragColor = out_color;
}  