#version 430

uniform sampler2D textureSampler;
uniform vec2 clip;
uniform float alphaValue;
in vec2 f_uv;

out vec4 pixelColor;

void main()
{
	vec2 textureUV = vec2(f_uv.x, 1.0 - f_uv.y);

	if(textureUV.x > clip.x)
		discard;
	if(textureUV.y > clip.y )
		discard;

	vec4 textureColor = texture(textureSampler, f_uv);

	if(textureColor.a <= 0.01) {
		discard;
	}
	else {
		pixelColor = textureColor;
	}

	pixelColor.a *= alphaValue;

}