#version 430

layout (location = 0) in vec3 position;

out vec3 textureCoords;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main()
{
    textureCoords = position;
    vec4 pos = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    gl_Position = position;
}