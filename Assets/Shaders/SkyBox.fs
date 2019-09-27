#version 440 core
out vec4 FragColor;

in vec3 TexCoords;
uniform samplerCube skyBox;

void main()
{
    FragColor = texture(skyBox, TexCoords);
}