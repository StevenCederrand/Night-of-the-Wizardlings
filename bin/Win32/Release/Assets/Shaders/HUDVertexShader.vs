#version 430

layout(location = 0) in vec4 vertex;
//layout(location = 1) in vec2 UV;

out vec2 textureCoords;
uniform mat4 projection;

void main() {

    gl_Position = projection * vec4(vertex.x, vertex.y, 0.0, 1.0);
    textureCoords = vertex.zw;
	
}
