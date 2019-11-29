#version 430

in vec2 textureCoords;

out vec4 out_Color;

uniform sampler2D text;
uniform vec3 textColor;
uniform float alpha;
void main(void){

	//out_Color = texture(text,textureCoords);
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, textureCoords).r);
	out_Color = vec4(textColor, alpha ) * sampled;

}
