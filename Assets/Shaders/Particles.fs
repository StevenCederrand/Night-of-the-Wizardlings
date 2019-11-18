#version 440

in vec2 UV_ps;
in float life_ps;

uniform sampler2D ps_texture;
uniform int glow;
uniform int fade;
uniform vec3 color;

out vec4 fragment_color;
void main() 
{
	fragment_color = texture(ps_texture, UV_ps);
	if (glow == 1)
	{
		fragment_color.xyz = vec3(0.0f, 1.0f, 0.0f);
	}

	//Fade in
	if(fade == -1)
	{
		fragment_color.w *= 1.0f -life_ps;
	}

	//Fade out
	if (fade == 1)
	{
		fragment_color.w *= life_ps;
	}

	fragment_color.xyz *= color;

	//fragment_color = vec4(0.0f, 0.5f, 1.0f, 1.0f);

	if (fragment_color.w < 0.1)
		discard;
}