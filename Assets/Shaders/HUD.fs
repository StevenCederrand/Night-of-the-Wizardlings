#version 430

uniform sampler2D textureSampler;
in vec2 texCoords;
out vec4 pixelColor;

uniform float alphaValue;


void main()
{
	vec4 textureColor = texture(textureSampler, texCoords);

	if(textureColor.a <= 0.01)
        discard;

	pixelColor = textureColor;
	pixelColor.a *= alphaValue;

}
