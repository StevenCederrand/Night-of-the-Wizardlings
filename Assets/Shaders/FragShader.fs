#version 430

out vec4 color;

void main() {
    vec2 xy = vec2(0.2f, 0.1f);
    color = vec4(xy, 1.2f, 1);
}
