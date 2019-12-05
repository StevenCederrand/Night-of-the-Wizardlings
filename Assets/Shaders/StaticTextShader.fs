#version 430

in vec2 f_uv;

out vec4 pixelColor;

uniform vec4 color;
uniform sampler2D fontAtlas;

void main(){
	vec4 texColor = texture(fontAtlas, f_uv);

	if(texColor.a <= 0.1)
		discard;
 

	pixelColor = color;
}