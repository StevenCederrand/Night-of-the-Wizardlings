#version 440

in vec2 UV_ps;
in float life_ps;

uniform sampler2D ps_texture;
uniform float glow;
uniform int fade;
uniform vec3 color;
uniform vec3 blendColor;

out vec4 fragment_color;
void main() 
{
	fragment_color = texture(ps_texture, UV_ps);// * 2;
	if (glow != 0)
	{
		fragment_color.xyz *= glow;
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
//----------	
	float mixValue = distance(UV_ps,vec2(0,1));
  	vec3 finalColor = mix(blendColor,color,mixValue);
//----------
	fragment_color.xyz *= finalColor;// * -life_ps; //color

	//fragment_color = vec4(0.0f, 0.5f, 1.0f, 1.0f);

	if (fragment_color.w < 0.1)
		discard;
}