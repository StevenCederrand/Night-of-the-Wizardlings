#version 430

in vec2 position;

out vec2 textureCoords;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main()
{
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position.x, 0.0, position.y, 1.0);
    textureCoords = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5);
}