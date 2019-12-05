#version 430

in vec2 f_uv;

out vec4 pixelColor;

uniform vec4 color;
uniform sampler2D fontAtlas;

const float width = 0.5;
const float edge = 0.1;

const float borderWidth = 0.6;
const float borderEdge = 0.2;

const vec3 outlineColor = vec3(0.0f, 0.0f, 0.0f);

void main(){
	pixelColor = color;

	float distance = 1.0 - texture(fontAtlas, f_uv).a;
	float alpha = 1.0 - smoothstep(width, width + edge, distance);
	
	float distanceOutline = 1.0 - texture(fontAtlas, f_uv).a;
	float outlineAlpha = 1.0 - smoothstep(borderWidth, borderWidth + borderEdge, distanceOutline);
	
	float overallAlpha = alpha + (1.0 - alpha) * outlineAlpha;
	
	vec3 overallColor = mix(outlineColor, pixelColor.xyz, alpha / overallAlpha);

	pixelColor = vec4(overallColor, overallAlpha);
	
}