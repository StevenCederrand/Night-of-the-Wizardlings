#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;


uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 f_position;
out vec3 f_normal;
out vec3 f_UV;
out vec3 f_R;


float bias = 1.0;
float scale = 1.0;
float power = 1.0;

void main()
{
    mat4 MVP = projMatrix * viewMatrix * modelMatrix;
    f_position = position * MVP;
    f_UV = uv;

    vec3 posWorld = (modelMatrix * position) * position;
    vec3 normWorld = normalize(mat3(modelMatrix*position) * normal);

    vec3 I = normalize(posWorld - viewMatrix.xyz);
    f_R = bias + scale * pow(1.0 + dot(I, normWorld), power);
}