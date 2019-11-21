#version 440

layout (points) in;
layout (triangle_strip, max_vertices=4) out;

in vec3 geometryDirection[];
in float geometryLife[];

uniform mat4 WVP;
uniform vec3 cam;
uniform vec2 size;
uniform int scaleDirection;
uniform int swirl;

out vec2 UV_ps;
out float life_ps;

void main()
{
	vec3 pos = gl_in[0].gl_Position.xyz; //+= geometryDirection[0] * 5;

	if(swirl == 1)
	{
		//pos += geometryDirection[0] * 5;
	}
	vec3 up = geometryDirection[0] * 2;
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

	if(swirl == 1)
	{
		//pos.x += cos(2 * 3.14 * geometryLife[0]); //check this
		//pos.z += sin(2 * 3.14 * geometryLife[0]); //check this
		//pos.y += geometryLife[0];// * -5; //these are mainly for the fire effect

		pos.x += 3.0f * cos(20 * 3.14 * geometryLife[0]);
		pos.z += 3.0f * sin(20 * 3.14 * geometryLife[0]);

	}

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

	//pos.x += 2 * cos(2 * 3.14 * geometryLife[0]); //check this
	//pos.z += 2 * sin(2 * 3.14 * geometryLife[0]); //check this
	//pos.y += geometryLife[0];// * -5; //these are mainly for the fire effect
	
	EndPrimitive();
}