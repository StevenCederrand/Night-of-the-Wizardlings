#version 430

in vec2 f_UV;
in vec3 f_normal;
in vec4 f_position;
out vec4 color;

vec3 lightDirection = vec3(0.5f, -1.0f, 0.0f);

uniform vec3 Ambient_Color;
uniform vec3 Diffuse_Color;
uniform vec3 Specular_Color;

uniform sampler2D albedoTexture;

void main() {
    float ambientStr = 0.15f;
    vec3 ambientCol = (Ambient_Color + ambientStr) * texture(albedoTexture, f_UV).rgb;

    vec3 lightDir = normalize(-lightDirection);
    float diff = max(dot(f_normal, lightDir), 0.0f);
    vec3 diffuse = (Diffuse_Color * texture(albedoTexture, f_UV).rgb) * diff;
    Specular_Color;

    color = texture(albedoTexture, f_UV); 
    color = vec4(ambientCol + diffuse, 1);
}
