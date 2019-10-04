#version 430
layout(std140, binding  = 1) uniform SkinDataBlock
{
	mat4 boneMat[64];
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in ivec4 bone;
layout(location = 4) in vec4 weight;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 f_UV;
out vec3 f_normal;
out vec4 f_position;
void main() {

    gl_Position  = vec4(position, 1.0f);
    gl_Position  = (boneMat[bone[0]] * vec4(position, 1.0f)) * weight.x;
	gl_Position += (boneMat[bone[1]] * vec4(position, 1.0f)) * weight.y;
	gl_Position += (boneMat[bone[2]] * vec4(position, 1.0f)) * weight.z;
	gl_Position += (boneMat[bone[3]] * vec4(position, 1.0f)) * weight.w;
    f_position = modelMatrix * gl_Position;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * gl_Position;
    
    f_normal  = vec3((boneMat[bone[0]] * vec4(normal, 1.0f)) * weight.x);
    f_normal += vec3((boneMat[bone[1]] * vec4(normal, 1.0f)) * weight.y);
    f_normal += vec3((boneMat[bone[2]] * vec4(normal, 1.0f)) * weight.z);
    f_normal += vec3((boneMat[bone[3]] * vec4(normal, 1.0f)) * weight.w);

    f_UV.x = uv.x;
    f_UV.y = -uv.y;
    f_normal = normal;
    f_normal =  normalize(transpose(inverse(mat3(modelMatrix))) * f_normal);
}
