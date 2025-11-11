#version 430 core

in vec3  val_color;
out vec4 FragColor;


uniform mat4 view;
uniform mat4 projection;

uniform sampler2D tex;
uniform bool use_texture = false;

void main() {
    // calculate legend texture color
    if(use_texture) {
        float val = length(val_color);
        // val is -10 ~ 10. convert to 0~1 about texture x-axis coordinate
        vec2 val_color = vec2(1.0 - val * 0.15);
        FragColor = texture(tex, val_color);
    }
    else{
        //  val_color is -10 ~ 10. convert to 0~1
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