#version 440

layout (points) in;
layout (triangle_strip, max_vertices=4) out;

in vec3 geometryDirection[];
in float geometryLife[];

uniform mat4 WVP;
uniform vec3 cam;
uniform vec2 size;
uniform int scaleDirection;

out vec2 UV_ps;
out float life_ps;

void main()
{
	vec3 pos = gl_in[0].gl_Position.xyz;
	vec3 up = geometryDirection[0];
	vec3 cam_normal = normalize(cam - pos);
	vec3 right = cross(cam_normal, up);

	up = up * size.y;
	right = right * size.x;

	if (scaleDirection == 1)
	{
		//Amp it up
		up *= 1.0f - geometryLife[0];
		right *= 1.0f - geometryLife[0];
	}

	if (scaleDirection == -1)
	{
		//Reduce size
		up *= geometryLife[0];
		right *= geometryLife[0];
	}

	life_ps = geometryLife[0];
	vec3 cPos = pos;

	cPos = pos - right - up;
	gl_Position = WVP * vec4(cPos, 1.0f);
	UV_ps = vec2(0.0f, 0.0f);
	EmitVertex();

	cPos = pos + right - up;
	gl_Position = WVP * vec4(cPos, 1.0f);
	UV_ps = vec2(1.0f, 0.0f);
	EmitVertex();

	cPos = pos + up - right;
	gl_Position = WVP * vec4(cPos, 1.0f);
	UV_ps = vec2(0.0f, 1.0f);
	EmitVertex();

	cPos = pos + up + right;
	gl_Position = WVP * vec4(cPos, 1.0f);
	UV_ps = vec2(1.0f, 1.0f);
	EmitVertex();
	
	EndPrimitive();
}