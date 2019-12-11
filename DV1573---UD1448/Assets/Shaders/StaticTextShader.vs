#version 430
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;


out vec2 f_uv;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

void main() {

    f_uv = uv;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}