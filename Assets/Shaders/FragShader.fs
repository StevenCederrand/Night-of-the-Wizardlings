#version 430

in vec2 f_UV;
in vec3 f_normal;
in vec4 f_position;
out vec4 color;

vec3 lightDirection = vec3(-.1f, -1.0f, -0.3f);

uniform sampler2D albedoTexture;

void main() {
    float ambientStr = 0.2f;
    vec3 ambientCol = vec3(1.0f, 1.0f, 1.0f) * ambientStr; //texture(albedoTexture, f_UV).rgb

    vec3 lightDir = normalize(-lightDirection);
    float diff = max(dot(f_normal, lightDir), 0.0f);
    vec3 diffuse = vec3(1.0f, 1.0f, 1.0f) * diff; //texture(albedoTexture, f_UV).rgb

    color = vec4(ambientCol + diffuse, 1);
}
