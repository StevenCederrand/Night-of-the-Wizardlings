#version 430

uniform sampler2D textureSampler;
in vec2 texCoords;
out vec4 pixelColor;

uniform float alphaValue;


void main()
{
	vec4 textureColor = texture(textureSampler, texCoords);
	pixelColor = textureColor;
	pixelColor.a *= alphaValue;

}
