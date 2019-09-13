#version 430

in vec2 f_UV;
out vec4 color;

uniform sampler2D albedoTexture;

void main() {
    vec2 xy = vec2(0.1f, 0.1f);
    color = texture(albedoTexture, f_UV);
}
