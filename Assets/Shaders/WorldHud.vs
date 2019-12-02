#version 430
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 uv;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 center;
uniform vec2 scale;

out vec2 f_uv;

void main() 
{
 	vec3 cameraRight = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
 	vec3 cameraUp = vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
 	vec3 newVertexPos = center + cameraRight * vertexPosition.x * scale.x + cameraUp * vertexPosition.y * scale.y;

 	gl_Position = projectionMatrix * viewMatrix * vec4(newVertexPos, 1.0f);
 	f_uv = uv;

}