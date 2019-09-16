#version 430

in vec2 f_UV;
in vec3 f_normal;
in vec4 f_position;
out vec4 color;

vec3 lightPos = vec3(1, 1, 0);

uniform sampler2D albedoTexture;

void main() {
    float ambientStr = 0.5f;
    vec4 textureCol = texture(albedoTexture, f_UV) * ambientStr;
    vec3 lightDir = normalize(lightPos - vec3(f_position));
    float diff = max(dot(f_normal, lightDir), 0.0f);

    textureCol *= diff;

    //vec4(f_normal, 1);
    color = textureCol;
}
