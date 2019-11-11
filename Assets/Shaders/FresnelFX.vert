#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
}