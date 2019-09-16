#version 430

in vec2 f_UV;
in vec3 f_normal;
out vec4 color;


uniform sampler2D albedoTexture;

void main() {
    vec2 xy = vec2(0.1f, 0.1f);
    color = vec4(f_normal, 1);//texture(albedoTexture, f_UV);
}
