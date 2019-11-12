
#version 430

layout(location = 0) in vec3 position;
//layout(location = 1) in vec3 normal;
layout(location = 1) in vec2 uv;

//out vec2 f_UV;
out vec2 texCoord;
void main(){
    
    //f_UV.x = uv.x;
    //f_UV.y = -uv.y;
    texCoord = uv;
    gl_Position = vec4(position, 1.0f);
    
}
