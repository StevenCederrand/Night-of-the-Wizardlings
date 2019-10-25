#version 430
#define LIGHTS_MAX 64

struct P_LIGHT {
    vec3 position;
    vec3 attenuation;
    float radius;
};

in vec2 f_UV;
in vec3 f_normal;
in vec4 f_position;
out vec4 color;

uniform vec3 Ambient_Color;
uniform vec3 Diffuse_Color;
uniform vec3 Specular_Color;
uniform bool HasTex;
uniform int LightCount;
uniform sampler2D albedoTexture;

uniform P_LIGHT pLights[LIGHTS_MAX];

void main()
{
	color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}