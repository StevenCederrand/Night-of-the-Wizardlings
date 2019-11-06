#version 430
out fragColor;

in f_position;
in f_normal;
in f_UV;
in f_R;

vec3 color = vec3(1.0);
vec3 fresColor = vec3(0.0, 0.0, 1.0);

void main()
{
    fragColor = lerp(color, fresColor, f_R);
}