#version 440

layout(location = 0) in vec3 vtxPos;
layout(location = 1) in vec3 vtxDir;
layout(location = 2) in float vtxLifetime;

out vec3 geometryDirection;
out float geometryLife;
			
void main () 
{
	gl_Position = vec4(vtxPos, 1);
	geometryDirection = vtxDir;
	geometryLife = vtxLifetime;
}