#version 430

in vec2 f_UV;
out vec4 color;

void main() {
    vec2 xy = vec2(0.1f, 0.1f);
    color = vec4(f_UV, 0, 1);
}
