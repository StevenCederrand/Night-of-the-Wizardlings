#version 430

uniform sampler2D textureSampler;
uniform vec2 clip;
uniform vec3 fillColor;
uniform float alphaValue;
uniform int grayscale;

in vec2 texCoords;
out vec4 pixelColor;



void main()
{
	vec2 sampler = vec2(texCoords.x, 1 - texCoords.y);

	vec4 textureColor = texture(textureSampler, texCoords);
	if(grayscale == 1) {
		float val = (textureColor.r + textureColor.g + textureColor.b) / 3;
		textureColor = vec4(val, val, val, textureColor.a);
	}

	if(textureColor.a <= 0.01) {
		discard;
	}
	else {
		pixelColor = textureColor;
	}

	if(sampler.x > clip.x)
		discard;
	if(sampler.y > clip.y )
		discard;

	pixelColor.a *= alphaValue;

}
