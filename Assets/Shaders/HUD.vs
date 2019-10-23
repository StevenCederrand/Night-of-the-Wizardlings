#version 430

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

out vec2 texCoords;

uniform mat4 modelMatrix;

void main() {

    texCoords = uv;
    gl_Position = modelMatrix * vec4(position, 0.0, 1.0);
    gl_Position.z = 0.0;
}
