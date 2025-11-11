#include "pch.h"
#include "GlslFiles.h"

/****************************** Default RSShader  ******************************/
const char* Default_vs = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    } 
)";
const char* Default_fs = R"(
    #version 330 core

    out vec4 FragColor;

    void main()
    {             
        FragColor = vec4(0.9, 0.9, 0.9, 1.0f);
    }  
)";

/****************************** General Shaders  ******************************/
const char* GeneralOpaqueSurface_vs = R"(
    #version 330 core
    #extension GL_ARB_explicit_attrib_location: enable
    #extension GL_ARB_explicit_uniform_location: enable
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 vert_color;
    layout (location = 2) in vec3 normal;

    uniform vec3 light_Dir;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec3 vert_color_frag;
    out float _diffuse;

    void main()
    {
        vec3 Worldnormal;
        vec3 worldLightPos;
        vec3 mlight_Dir = vec3(0, 0, 0);
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        vert_color_frag = vert_color;
        //gl_PointSize = 3; //point로 렌더링 시 point크기
        Worldnormal = mat3(model) * normal;
    
        mlight_Dir = normalize(light_Dir);
        Worldnormal = normalize(Worldnormal);
        float diff = max(dot(Worldnormal, -mlight_Dir), 0.0);

        _diffuse = diff;

    } 
)";
const char* GeneralOpaqueSurface_fs = R"(
    #version 330 core

    in vec3 vert_color_frag;
    in float _diffuse;
    out vec4 FragColor;

    void main()
    {             
        float ambient = 0.5f;

        FragColor = clamp(ambient + _diffuse*0.25f, 0.0, 1.0) * vec4(vec3(vert_color_frag), 1.0f);
    }  
)";

const char* GeneralOpaqueLine_vs = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    } 
)";
const char* GeneralOpaqueLine_fs = R"(
    #version 330 core

    uniform vec3 face_color;

    out vec4 FragColor;

    void main()
    {             
        FragColor = vec4(vec3(face_color), 1.0f);
    }  
)";


const char* GeneralTransParentSurface_vs = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vert_color;
layout (location = 2) in vec3 normal;

uniform vec3 light_Dir;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 vert_color_frag;
out float _diffuse;
out vec4 _position;

void main()
{
    vec3 Worldnormal;
    vec3 worldLightPos;
    vec3 mlight_Dir = vec3(0, 0, 0);
    _position = model * vec4(aPos, 1.0);

    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vert_color_frag = vec4(vert_color,1.0f);
    
    Worldnormal = mat3(model) * normal;
    
    mlight_Dir = normalize(light_Dir);
    Worldnormal = normalize(Worldnormal);
    float diff = max(dot(Worldnormal, -mlight_Dir), 0.0);

    _diffuse = diff;
} 
)";

const char* GeneralTransParentSurface_fs = R"(
#version 330 core

layout (location = 0) out vec4 accum;
layout (location = 1) out float reveal;

uniform float alpha;
uniform vec3 point;
uniform vec3 normalvec;

in vec4 vert_color_frag;
in float _diffuse;
//out vec4 FragColor;

void main()
{             
    //FragColor = clamp(ambient + _diffuse*0.25f, 0.0, 1.0) * vec4(vec3(vert_color_frag) * alpha, alpha) * weight;

    float ambient = 0.5f;
	// weight function
	float weight = clamp(pow(min(1.0, alpha * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-1, 3e1);

	// store pixel color accumulation
	//accum = clamp(ambient + _diffuse*0.25f, 0.0, 1.0)  * vec4(vert_color_frag.rgb * alpha, alpha) * weight;
	accum =  vec4(clamp(ambient + _diffuse*0.25f, 0.0, 1.0) * vert_color_frag.rgb * alpha, alpha) * weight;
	// store pixel revealage threshold
	reveal = alpha;
} 
)";

const char* GeneralTransParentLine_vs = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    } 
)";
const char* GeneralTransParentLine_fs = R"(
    #version 330 core
    layout (location = 0) out vec4 accum;
    layout (location = 1) out float reveal;

    uniform vec3 face_color;
    uniform float alpha;

    void main()
    {             
	    // weight function
	    float weight = clamp(pow(min(1.0, alpha * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-1, 2e1);
	
	    // store pixel color accumulation
	    accum = vec4(face_color.rgb * alpha, alpha) * weight;
	
	    // store pixel revealage threshold
	    reveal = alpha;
    }  
)";

/****************************** Control Shaders  ******************************/
const char* ControlOpaqueSurface_vs = R"(
    #version 330 core
    #extension GL_ARB_explicit_attrib_location: enable
    #extension GL_ARB_explicit_uniform_location: enable
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 vert_color;
    layout (location = 2) in vec3 normal;

    uniform vec3 light_Dir;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;


    //uniform mat4 model;

    out vec4 vert_color_frag;
    out float _diffuse;
    out vec4 _position;

    void main()
    {
        vec3 Worldnormal;
        vec3 worldLightPos;
        vec3 mlight_Dir = vec3(0, 0, 0);
        _position = model * vec4(aPos, 1.0);

        gl_Position = projection * view * model * vec4(aPos, 1.0);

        vert_color_frag = vec4(vert_color, 1.0f);
    
        Worldnormal = mat3(model) * normal;
    
        mlight_Dir = normalize(light_Dir);
        Worldnormal = normalize(Worldnormal);
        float diff = max(dot(Worldnormal, -mlight_Dir), 0.0);

        _diffuse = diff;

    } 
)";
const char* ControlOpaqueSurface_fs = R"(
    #version 330 core

    uniform vec3 point;
    uniform vec3 normalvec;

    in vec4 vert_color_frag;
    in float _diffuse;
    in vec4 _position;
    out vec4 FragColor;

    void main()
    {             
        float ambient = 0.5f;
        float d = -dot(point, normalvec);
        float abover = dot(_position.xyz, normalvec) + d;

        if(abover < 0.0)
        {
            discard;
        }
        FragColor = clamp(ambient + _diffuse*0.25f, 0.0, 1.0) * vert_color_frag;
    }  
)";


const char* ControlOpaqueLine_vs = R"(
    #version 330 core
    #extension GL_ARB_explicit_attrib_location: enable
    #extension GL_ARB_explicit_uniform_location: enable
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 vert_color;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec4 _position;

    void main()
    {
        _position = model * vec4(aPos, 1.0);
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    } 
)";
const char* ControlOpaqueLine_fs = R"(
    #version 330 core

    uniform vec3 face_color;
    uniform vec3 point;
    uniform vec3 normalvec;

    in vec4 _position;
    out vec3 FragColor;

    void main()
    {                
        float ambient = 0.5f;
        float d = -dot(point, normalvec);
        float abover = dot(_position.xyz, normalvec) + d;

        if(abover <= 0.0)
        {
            discard;
        }

        FragColor = face_color;
    }  
)";



const char* ControlTransParentSurface_vs = R"(
#version 330 core
#extension GL_ARB_explicit_attrib_location: enable
#extension GL_ARB_explicit_uniform_location: enable
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vert_color;
layout (location = 2) in vec3 normal;

uniform vec3 light_Dir;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 vert_color_frag;
out float _diffuse;
out vec4 _position;

void main()
{
    vec3 Worldnormal;
    vec3 worldLightPos;
    vec3 mlight_Dir = vec3(0, 0, 0);
    _position = model * vec4(aPos, 1.0);

    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vert_color_frag = vec4(vert_color,1.0f);
    
    Worldnormal = mat3(model) * normal;
    
    mlight_Dir = normalize(light_Dir);
    Worldnormal = normalize(Worldnormal);
    float diff = max(dot(Worldnormal, -mlight_Dir), 0.0);

    _diffuse = diff;
} 
)";
const char* ControlTransParentSurface_fs = R"(
#version 330 core

layout (location = 0) out vec4 accum;
layout (location = 1) out float reveal;

uniform float alpha;
uniform vec3 point;
uniform vec3 normalvec;

in vec4 vert_color_frag;
in float _diffuse;
in vec4 _position;

    void main()
    {             
        float d = -dot(point, normalvec);
        float abover = dot(_position.xyz, normalvec) + d;

        if(abover > 0.0)
        {
            discard;
        }

        float ambient = 0.5f;
	    // weight function
	    float weight = clamp(pow(min(1.0, alpha * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-1, 3e1);

	    // store pixel color accumulation
	    //accum = clamp(ambient + _diffuse*0.25f, 0.0, 1.0)  * vec4(vert_color_frag.rgb * alpha, alpha) * weight;
	    accum =  vec4(clamp(ambient + _diffuse*0.25f, 0.0, 1.0) * vert_color_frag.rgb * alpha, alpha) * weight;
	    // store pixel revealage threshold
	    reveal = alpha;
    }

)";


const char* ControlTransParentLine_vs = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vert_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 _position;

void main()
{
    _position = model * vec4(aPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
} 

)";

const char* ControlTransParentLine_fs = R"(
#version 330 core

uniform float alpha;
uniform vec3 line_color;
uniform vec3 point;
uniform vec3 normalvec;

in vec4 vert_color_frag;
in vec4 _position;
out vec4 FragColor;

void main()
{             
    float d = -dot(point, normalvec);
    float abover = dot(_position.xyz, normalvec) + d;
    if(abover > 0.0)
    {
        discard;
    }
    FragColor.xyz = line_color.xyz; 
    FragColor.a = alpha;
    
}  
)";


/****************************** Tool Shaders  ******************************/
const char* ToolOpaque_vs = R"(
#version 330 core
#extension GL_ARB_explicit_attrib_location: enable
#extension GL_ARB_explicit_uniform_location: enable
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
} 

)";
const char* ToolOpaque_fs = R"(
#version 330 core

uniform vec3 color;

out vec4 FragColor;

void main()
{             
    FragColor = vec4(color, 1);
}  

)";


const char* ToolTransparent_vs = R"(
#version 330 core
#extension GL_ARB_explicit_attrib_location: enable
#extension GL_ARB_explicit_uniform_location: enable
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vert_color;
layout (location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 vert_color_frag;
out float _diffuse;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vert_color_frag = vec4(1, 1, 1, 0.5f);
} 

)";
const char* ToolTransparent_fs = R"(
#version 330 core

uniform vec3 color;
uniform float alpha;
in vec4 vert_color_frag;
out vec4 FragColor;

void main()
{             
    FragColor = vec4(color, alpha);
}  

)";


/****************************** Etc Shaders  ******************************/
const char* PickedSurface_vs = R"(
#version 330 core
#extension GL_ARB_explicit_attrib_location: enable
#extension GL_ARB_explicit_uniform_location: enable
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vert_color;
layout (location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
} 

)";
const char* PickedSurface_fs = R"(
#version 330 core

uniform vec3 color_;
out vec4 FragColor;

void main()
{             
    FragColor = vec4(color_, 1.0f);
}  
)";


const char* AimPoint_vs = R"(
    #version 330 core
    #extension GL_ARB_explicit_attrib_location: enable
    #extension GL_ARB_explicit_uniform_location: enable
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 vert_color;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;


    out vec3 vert_color_frag;
    out vec4 pos;

    void main()
    {
	    vec4 position;
	    position = projection * view * model * vec4(aPos, 1.0);
        gl_Position = position;
	    pos = projection * view * model * vec4(aPos, 1.0);;
        vert_color_frag = vert_color;

    } 
)";
const char* AimPoint_fs = R"(
    #version 330 core

    in vec3 vert_color_frag;
    in vec4 pos;

    out vec4 FragColor;

    uniform vec3 color_ = vec3(1, 1, 1);

    void main()
    {             
        FragColor = vec4(color_, 1);
    }  
)";


const char* Composit_vs = R"(
#version 400 core

layout (location = 0) in vec3 position;

void main()
{
	gl_Position = vec4(position, 1.0f);
}
)";

const char* Composit_fs = R"(
#version 400 core
#extension GL_ARB_shading_language_420pack : enable

// shader outputs
layout (location = 0) out vec4 frag;

// color accumulation buffer
layout (binding = 0) uniform sampler2D accum;

// revealage threshold buffer
layout (binding = 1) uniform sampler2D reveal;

// epsilon number
const float EPSILON = 0.00001f;

// calculate floating point numbers equality accurately
bool isApproximatelyEqual(float a, float b)
{
	return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

// get the max value between three values
float max3(vec3 v) 
{
	return max(max(v.x, v.y), v.z);
}

void main()
{
	// fragment coordination
	ivec2 coords = ivec2(gl_FragCoord.xy);
	
	// fragment revealage
	float revealage = texelFetch(reveal, coords, 0).r;
	
	// save the blending and color texture fetch cost if there is not a transparent fragment
	if (isApproximatelyEqual(revealage, 1.0f)) 
		discard;
 
	// fragment color
	vec4 accumulation = texelFetch(accum, coords, 0);
	
	// suppress overflow
	if (isinf(max3(abs(accumulation.rgb)))) 
		accumulation.rgb = vec3(accumulation.a);

	// prevent floating point precision bug
	vec3 average_color = accumulation.rgb / max(accumulation.a, EPSILON);

	// blend pixels
	frag = vec4(average_color, 1.0f - revealage);
}
)";

//Screen
const char* Screen_vs = R"(
#version 400 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 texture_coords;

void main()
{
	texture_coords = uv;

	gl_Position = vec4(position, 1.0f);
}
)";

const char* Screen_fs = R"(
#version 400 core

in vec2 texture_coords;

layout (location = 0) out vec4 frag;

uniform sampler2D screen;

void main()
{
	frag = vec4(texture(screen, texture_coords).rgb, 1.0f);
}
//#version 400 core
//
//in vec2 texture_coords;
//
//layout (location = 0) out vec4 frag;
//
//uniform sampler2D screen;
//uniform sampler2D hud;
//
//void main()
//{
//    vec4 screenColor = texture(screen, texture_coords);
//    vec4 hudColor = texture(hud, texture_coords);
//
//    // HUD의 알파 값이 0.1보다 작을 때는 스크린 이미지의 색상 사용
//    vec4 finalColor = (hudColor.a < 0.1) ? screenColor : hudColor;
//
//    frag = vec4(finalColor.rgb, 1.0f);
//}
)";

//Text
const char* HudText_vs = R"(
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)";

const char* HudText_fs = R"(
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}
)";

//RSTexture
const char* HudTexture_vs = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 acolor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
	gl_Position = model * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
)";

const char* HudTexture_fs = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    vec2 Texcoord_flipy = vec2( TexCoord.x, 1.0 - TexCoord.y);
	FragColor = texture(texture1, Texcoord_flipy);
}
)";

/****************************** Test Shaders  ******************************/
const char* Hud_vs = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoords;

    out vec2 TexCoords;

    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
        TexCoords = aTexCoords;
    }
)";
const char* Hud_fs = R"(
    #version 330 core
    out vec4 FragColor;

    in vec2 TexCoords;

    uniform sampler2D hudTexture;

    void main()
    {
        vec4 sampled = texture(hudTexture, TexCoords);
        FragColor = sampled;
    }
)";

const char* Depth_vs = R"(
    #version 330 core
    #extension GL_ARB_explicit_attrib_location: enable
    #extension GL_ARB_explicit_uniform_location: enable
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 vert_color;
    layout (location = 2) in vec3 normal;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    } 
)";
const char* Depth_fs = R"(
    #version 330 core

    uniform float near;
    uniform float far;

    out vec4 FragColor;

    float LinearizeDepth(float depth) 
    {
        float z = depth * 2.0 - 1.0; // back to NDC 
        return (2.0 * near * far) / (far + near - z * (far - near));	
    }

    void main()
    {             
        //float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
        //float depth = -log(LinearizeDepth(gl_FragCoord.z) / far); // divide by far for demonstration
        float depth = (gl_FragCoord.z - 0.99) * 100 + 0.5;
        vec2 pos = gl_FragCoord.xy/800;
        //depth = clamp(depth, 0.2, 0.8);
        FragColor = vec4(vec3(depth*depth, depth, 1.0 - depth), 1.0);
        //FragColor = vec4(vec3(depth, depth, depth), 1.0);
        //FragColor = vec4(vec3(pos.x, pos.y, depth), 1.0);
        //FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    }  
)";


const char* Skybox_vs = R"(
#version 330 core
layout (location = 0) in vec3 vertex;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = vertex;
	vec4 pos = projection * view * vec4(vertex, 1.0);
	gl_Position = pos.xyww;
}
)";
const char* Skybox_fs = R"(
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}
)";

const char* PointRenderShader_vs = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    } 
)";
const char* PointRenderShader_fs = R"(
    #version 330 core

    uniform vec3 color;

    out vec4 FragColor;

    void main()
    {             
        FragColor = vec4(vec3(color), 1.0f);
    }  
)";


const char* PointInstancingCubeRenderShader_vs = R"(
    #version 330 core
    layout (location = 0) in vec3 pos;
    layout (location = 1) in vec3 normal;
    layout (location = 2) in vec3 translate;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    uniform vec3 light_Dir;
    out float _diffuse;

    void main()
    {
        vec3 Worldnormal = mat3(model) * normal;
        vec3 mlight_Dir = vec3(0, 0, 0);

        mlight_Dir = normalize(light_Dir);
        Worldnormal = normalize(Worldnormal);
        float diff = max(dot(Worldnormal, -mlight_Dir), 0.0);

        _diffuse = diff;
        gl_Position = projection * view * model * vec4(pos+translate, 1.0);
    } 
)";
const char* PointInstancingCubeRenderShader_fs = R"(
    #version 330 core

    uniform vec3 color;

    in float _diffuse;
    out vec4 FragColor;

    void main()
    {             
        float ambient = 0.5f;
        FragColor = clamp(ambient + _diffuse*0.25f, 0.0, 1.0) * vec4(vec3(color), 1.0f);
    }  
)";

const char* PointToCubeShader_gs = R"(
#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

uniform float size = 0.02; // change to uniform
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

 vec3 cubeVertices[8] = vec3[](
    vec3(-size, -size, -size),
    vec3(size, -size, -size),
    vec3(size, size, -size),
    vec3(-size, size, -size),
    vec3(-size, -size, size),
    vec3(size, -size, size),
    vec3(size, size, size),
    vec3(-size, size, size)
);

const int cubeIndices[36] = int[](
    0, 1, 2, 2, 3, 0, // 앞면
    4, 5, 6, 6, 7, 4, // 뒷면
    3, 2, 6, 6, 7, 3, // 위쪽 면
    0, 1, 5, 5, 4, 0, // 아래쪽 면
    1, 2, 6, 6, 5, 1, // 오른쪽 면
    0, 3, 7, 7, 4, 0  // 왼쪽 면
);

void main() {
    vec3 position = gl_in[0].gl_Position.xyz;

    for (int i = 0; i < 36; i++) {
        vec3 vertexPos = position + cubeVertices[cubeIndices[i]];

        gl_Position = projection * view * model * vec4(vertexPos, 1.0);
        EmitVertex();

        if (i % 3 == 2) {
            EndPrimitive();
        }
    }
}
)";

const char* PointToCubeShader_vs = R"(
    #version 330 core
    layout (location = 0) in vec3 pos;


    void main()
    {
        gl_Position = vec4(pos, 1.0);
    } 
)";

// Frag RSShader에서 lighting 계산. 꼭 필요한 기능인가?
const char* PointToCubeShader_fs = R"(
    #version 330 core

    uniform vec3 color;
    out vec4 FragColor;

    void main()
    {             
        FragColor = vec4(vec3(color), 1.0f);
    }  
)";


const char* PointToBillboardTextureGeometryShader_vs = R"(
    #version 330 core
    layout (location = 0) in vec3 pos;

    void main()
    {
        gl_Position = vec4(pos, 1.0);
    } 
)";

const char* PointToBillboardTextureGeometryShader_gs = R"(
    #version 330 core

    layout(points) in;
    layout(triangle_strip, max_vertices = 4) out;

    uniform float size = 0.02; // change to uniform
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    out vec2 UV;

    void main() {
        vec3 position = gl_in[0].gl_Position.xyz;
        vec3 viewDir = normalize(vec3(view[0][2], view[1][2], view[2][2]));
        vec3 up = vec3(view[0][1], view[1][1], view[2][1]);
        vec3 right = cross(viewDir, up); 

        vec3 v1 = position + (-right - up) * size;
        vec3 v2 = position + (right - up) * size;
        vec3 v3 = position + (-right + up) * size;
        vec3 v4 = position + (right + up) * size;

        gl_Position = projection * view * model * vec4(v1, 1.0);
        UV = vec2(0, 0);
        EmitVertex();

        gl_Position = projection * view * model * vec4(v2, 1.0);
        UV = vec2(1, 0);
        EmitVertex();

        gl_Position = projection * view * model * vec4(v3, 1.0);
        UV = vec2(0, 1);
        EmitVertex();

        gl_Position = projection * view * model * vec4(v4, 1.0);
        UV = vec2(1, 1);
        EmitVertex();

        EndPrimitive();
    }
)";

const char* PointToBillboardTextureGeometryShader_fs = R"(
    #version 330 core
    in vec2 UV;
    out vec4 FragColor;
    uniform sampler2D texture_;

    void main() {
        vec2 Texcoord_flipy = vec2(UV.x, 1.0 - UV.y);
        vec4 tex_color = texture(texture_, Texcoord_flipy);
        if(tex_color.a < 0.1) 
            discard;
        FragColor = tex_color;
    }
)";


const char* PointToSurfaceExtandShader_vs = R"(
    #version 330 core
    layout (location = 0) in vec3 pos;
    layout (location = 1) in uint flag_in;

    out uint flag;
    void main()
    {
        flag = flag_in;
        gl_Position = vec4(pos, 1.0);
    } 
)";

const char* PointToSurfaceExtandShader_gs = R"(
#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;
in uint flag[];

uniform float size = 0.02; // change to uniform
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
   
vec3 adjust_pos[6];
void main() {
    adjust_pos[0] = vec3(1, 0, 0);
    adjust_pos[1] = vec3(-1, 0, 0);
    adjust_pos[2] = vec3(0, 0, -1);
    adjust_pos[3] = vec3(0, 0, 1);
    adjust_pos[4] = vec3(0, 1, 0);
    adjust_pos[5] = vec3(0, -1, 0);

    vec3 position = gl_in[0].gl_Position.xyz;
        
    uint comparing = uint(0x00000001);
    uint flag_value = uint(flag[0]);
    for(int i = 0; i < 6; i++)
    {
        if((flag_value & comparing) == 0u)
        {
            vec3 base = adjust_pos[i];
            vec3 vectors[4];

            int v = 0;
            for (int a = -1; a <= 1; a += 2) {
                for (int b = -1; b <= 1; b += 2) {
                    if (base.x != 0) {
                        vectors[v++] = vec3(base.x, a, b);
                    } else if (base.y != 0) {
                        vectors[v++] = vec3(a, base.y, b);
                    } else {
                        vectors[v++] = vec3(a, b, base.z);
                    }
                }
            }

            vec3 v1 = position + vectors[0] * size;
            vec3 v2 = position + vectors[1] * size;
            vec3 v3 = position + vectors[2] * size;
            vec3 v4 = position + vectors[3] * size;

            gl_Position = projection * view * model * vec4(v1, 1.0);
            EmitVertex();

            gl_Position = projection * view * model * vec4(v2, 1.0);
            EmitVertex();

            gl_Position = projection * view * model * vec4(v3, 1.0);
            EmitVertex();

            gl_Position = projection * view * model * vec4(v4, 1.0);
            EmitVertex();
            
            EndPrimitive();
        }
        comparing <<= 4;
    }
    //EndPrimitive();
}
)";

const char* PointToSurfaceExtandShader_fs = R"(
    #version 330 core

    uniform vec3 color;
    out vec4 FragColor;

    void main()
    {             
        FragColor = vec4(vec3(color), 1.0f);
    }  
)";


const char* Occlusion_Query_vs = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    } 
)";


const char* Occlusion_Query_fs = R"(
    #version 330 core
    void main()
    {             
        //NOTHING. JUST CHECK Z
    }  
)";


//const char* PointInstancingRenderShader_vs = R"(
//    #version 330 core
//    layout (location = 0) in vec3 aPos;
//    layout (location = 1) in vec3 aOffset;
//
//    uniform mat4 model;
//    uniform mat4 view;
//    uniform mat4 projection;
//
//    void main()
//    {
//        gl_Position = projection * view * model * vec4(aPos + aOffset, 1.0);
//    } 
//)";
//const char* PointInstancingRenderShader_fs = R"(
//    #version 330 core
//
//    uniform vec3 color;
//
//    out vec4 FragColor;
//
//    void main()
//    {             
//        FragColor = vec4(vec3(color), 1.0f);
//    }  
//)";

const char* DefaultComputeShader_cs = R"(
#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main() {
    // NOTHING
}
)";

const char* SampleComputeShader_cs = R"(
#version 450

layout(local_size_x = 1024) in;

layout(std430, binding = 0) buffer Input {
    vec4 points[];
};

layout(std430, binding = 1) buffer Output {
    vec4 front_points[];
};

layout(binding = 0, offset = 0) uniform atomic_uint acCounter;

void main() {
    uint idx = gl_GlobalInvocationID.x;

    if (idx < points.length()) {
        if (idx % 99 == 0) {
            uint count = atomicCounterIncrement(acCounter);
            front_points[count] = vec4(vec3(points[idx]), 1.0);
        }
    }
}
)";



const char* PointToBillboardTextureComputeShader_vs = R"(
#version 450

layout(std430, binding = 1) buffer Output {
    vec4 front_points[];
};

void main()
{
    vec4 position = front_points[gl_VertexID];
    gl_Position = position;
} 
)";

